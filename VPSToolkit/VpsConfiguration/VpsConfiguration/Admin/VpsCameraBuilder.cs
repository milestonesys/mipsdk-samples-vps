using System;
using System.Collections.Generic;
using System.Linq;
using VideoOS.Platform;
using VideoOS.Platform.ConfigurationItems;

namespace VpsConfiguration.Admin
{
    /// <summary>
    /// This class will take care of the lowest level issues when creating new virtual VPS cameras.
    /// You should not consider looking into changing this code unless you have special needs. For starters, keep this code as it is.
    /// </summary>
    /// 
    public class VpsCameraBuilder
    {
        /// <summary>
        /// Get the Home site addresses.
        /// An address with the same port as the current environment Management Server address will be on top.
        /// </summary>
        public static IEnumerable<string> GetSiteAddressesOrdered()
        {
            ManagementServer managementServer = new ManagementServer(Configuration.Instance.ServerFQID.ServerId);
            var addresses = managementServer.SystemAddressFolder.SystemAddressChildItems.Select(c => c.Url);

            var loginSettings = VideoOS.Platform.Login.LoginSettingsCache.GetLoginSettings(Configuration.Instance.ServerFQID);

            // Remove HTTPS addresses until Driver supports it
            addresses = addresses.Where(a => !a.StartsWith(Uri.UriSchemeHttps));

            addresses = addresses
                    .OrderByDescending(a => new Uri(a).Port == loginSettings.Uri.Port)
                    .ToList();

            return addresses;
        }

        /// <summary>
        /// The BuildCameraForEachChild method creates a new VPS hardware for each source camera in an input group of cameras.
        /// </summary>
        /// <param name="itemSelectedCameraGroup">The input camera group</param>
        /// <param name="cameraFormat">The format to use when creating the new hardwares</param>
        /// <param name="cameraGroupName">The name of a camera group to put it into. Will be created if it does not exist.</param>
        /// <param name="cameraSuffix">The suffix to add to the camera device of the new hardware</param>
        /// <param name="metadataGroupName">The name of a metadata group to put it into. Will be created if it does not exist</param>
        /// <param name="metadataSuffix">The suffix to add to the metadata device of the new hardware</param>
        /// <param name="managementServerUrl">The Management Server URL specifying port and scheme to use.</param>
        /// <param name="vpsServiceUrls">A comma separated list of URLs describing the VPS Service. Use one URL if you use your own load balancer</param>
        /// <param name="gstreamerPipelineConfig">If you have the processing stream configuration in the XProtect management client, pass it here. Otherwise leave empty</param>
        public static void BuildCameraForEachChild(
            Item itemSelectedCameraGroup, 
            string cameraFormat,
            string cameraGroupName,
            string cameraSuffix,
            string metadataGroupName,
            string metadataSuffix,
            string managementServerUrl,
            string vpsServiceUrls,
            string gstreamerPipelineConfig)
        {
            List<Item> cameraItems = itemSelectedCameraGroup.GetChildren();
            foreach (Item cameraItem in cameraItems)
            {
                if (cameraItem.FQID.Kind == Kind.Camera && cameraItem.FQID.FolderType == FolderType.No)
                {
                    string cameraName = cameraFormat.Replace("###CAMERA_NAME###", cameraItem.Name);
                    BuildOneCamera(cameraItem, cameraName, cameraGroupName, cameraSuffix, metadataGroupName, metadataSuffix, managementServerUrl, vpsServiceUrls, gstreamerPipelineConfig);
                }
            }
        }
        /// <summary>
        /// The BuildOneCamera method creates a new VPS hardware for one input source camera
        /// </summary>
        /// <param name="itemSelectedCameraGroup">The input camera The name of the new hardware to create</param>
        /// <param name="hardwareName">The name of the new hardware to create</param>
        /// <param name="cameraGroupName">The name of a camera group to put it into. Will be created if it does not exist</param>
        /// <param name="cameraSuffix">The suffix to add to the camera device of the new hardware</param>
        /// <param name="metadataGroupName">The name of a metadata group to put it into. Will be created if it does not exist</param>
        /// <param name="metadataSuffix">The suffix to add to the metadata device of the new hardware</param>
        /// <param name="managementServerUrl">The Management Server URL specifying port and scheme to use.</param>
        /// <param name="vpsServiceUrls">A comma separated list of URLs describing the VPS Service. Use one URL if you use your own load balancer</param>
        /// <param name="gstreamerPipelineConfig">If you have the processing stream configuration in the XProtect management client, pass it here. Otherwise leave empty</param>
        public static void BuildOneCamera(Item itemSelectedCamera,
            string hardwareName,
            string cameraGroupName,
            string cameraSuffix,
            string metadataGroupName,
            string metadataSuffix,
            string managementServerUrl,
            string vpsServiceUrls,
            string gstreamerPipelineConfig)
        {
            // get an instance of the source camera first, if it's been deleted this will throw an exception
            Camera sourceCamera = new Camera(itemSelectedCamera.FQID);

            // Add a new hardware device on the XProtect management server
            string username = string.Empty; // Setting username empty will create the hardware with the rights of the user running the recording server
            string password = string.Empty;
            ManagementServer managementServer = new ManagementServer(Configuration.Instance.ServerFQID.ServerId);
            Guid recordingServerId = itemSelectedCamera.FQID.ParentId;
            Guid hardwareId = AddHardware(managementServerUrl, recordingServerId, username, password, hardwareName, cameraGroupName, metadataGroupName);

            // Name and enable the new hardware
            Hardware hardware = new Hardware(EnvironmentManager.Instance.MasterSite.ServerId, $"Hardware[{hardwareId.ToString()}]");
            hardware.Name = hardwareName;
            hardware.Enabled = true;
            hardware.Save();

            // Populate the camera device in the new hardware with relevant properties
            ConfigureHardware(hardware, itemSelectedCamera.FQID.ObjectId.ToString(), vpsServiceUrls, gstreamerPipelineConfig);

            // Name and enable the new camera device
            Camera camera = hardware.CameraFolder.Cameras.First();
            camera.Name = $"{hardwareName} - {cameraSuffix}";
            camera.Enabled = true;
            camera.Save();

            // Name and enable the new metadata device
            Metadata metadata = hardware.MetadataFolder.Metadatas.First();
            metadata.Name = $"{hardwareName} - {metadataSuffix}";
            metadata.Enabled = true;
            metadata.Save();

            // Add the new camera device to a specific camera group.
            CameraGroup cameraGroup = managementServer.CameraGroupFolder.CameraGroups.FirstOrDefault(g => g.Name == cameraGroupName);
            if (cameraGroup == null)
            {
                // Create the camera group if it does not already exist
                AddDeviceGroupServerTask task = managementServer.CameraGroupFolder.AddDeviceGroup();
                task.GroupName = cameraGroupName;
                task.GroupDescription = "";
                ServerTask result = task.Execute();
                while (true)
                {
                    StateEnum state = result.State;
                    if (state == StateEnum.Error || state == StateEnum.Success)
                    {
                        break;
                    }
                    System.Threading.Thread.Sleep(500);
                    result.UpdateState();
                }
                if (result.State != StateEnum.Error)
                {
                    cameraGroup = new CameraGroup(Configuration.Instance.ServerFQID.ServerId, result.Path);
                }
            }

            // Don't fail if the camera group was not created. It does not prevent the process from running, it is merely convenience for the end user.
            cameraGroup?.CameraFolder.AddDeviceGroupMember(camera.Path);

            // Add the new metadata device to a specific metadata group.
            MetadataGroup metadataGroup = managementServer.MetadataGroupFolder.MetadataGroups.FirstOrDefault(g => g.Name == metadataGroupName);
            if (metadataGroup == null)
            {
                // Create the metadata group if it does not already exist
                AddDeviceGroupServerTask task = managementServer.MetadataGroupFolder.AddDeviceGroup();
                task.GroupName = metadataGroupName;
                task.GroupDescription = "";
                ServerTask result = task.Execute();
                while (true)
                {
                    StateEnum state = result.State;
                    if (state == StateEnum.Error || state == StateEnum.Success)
                    {
                        break;
                    }
                    System.Threading.Thread.Sleep(500);
                    result.UpdateState();
                }
                if (result.State != StateEnum.Error)
                {
                    metadataGroup = new MetadataGroup(Configuration.Instance.ServerFQID.ServerId, result.Path);
                }
            }

            // Don't fail if the metadata group was not created. It does not prevent the process from running, it is merely convenience for the end user.
            metadataGroup?.MetadataFolder.AddDeviceGroupMember(metadata.Path);

            // Set the newly created metadata device as "related metadata device" for the original source camera device.
            // This will cause the computed metadata's bounding boxes to appear in the Smart Client.
            ClientSettings clientSettings = sourceCamera.ClientSettingsFolder.ClientSettings.FirstOrDefault();
            clientSettings.Related += $",{metadata.Path}";
            clientSettings.Save();
        }
        private static Guid AddHardware(string managementServerUrl, Guid recordingServerId, string user, string pass, string hardwareName, string cameraGroup, string metadataGroup)
        {
            // Get the management server
            ManagementServer managementServer = new ManagementServer(EnvironmentManager.Instance.MasterSite);

            // Create this new hardware on the same recording server as the source camera
            RecordingServer recordingServer = managementServer.RecordingServerFolder.RecordingServers.FirstOrDefault(x => x.Id.ToUpper() == recordingServerId.ToString().ToUpper());
            if (recordingServer == null)
            {
                // But if you can't find that recording server, any server will do
                recordingServer = managementServer.RecordingServerFolder.RecordingServers.First();
            }
            if (recordingServer == null)
            {
                throw new MIPException("Error. Did not find a recording server.");
            }

            // The VPS driver has the internal number 20002. (If you have one with 20001, it is obsolete)
            string hardwareDriverPath = recordingServer.HardwareDriverFolder.HardwareDrivers.Where(x => x.Number == 20002).FirstOrDefault()?.Path;
            if (hardwareDriverPath == null)
            {
                throw new MIPException("Error. Did not find the VPS driver with index 20002");
            }

            // Now, create the actual new VPS hardware
            ServerTask addHardwareServerTask = recordingServer.AddHardware(managementServerUrl, hardwareDriverPath, user, pass);

            // Loop around, checking for the craetion task to finish
            while (addHardwareServerTask.State != StateEnum.Error && addHardwareServerTask.State != StateEnum.Success)
            {
                System.Threading.Thread.Sleep(500);
                addHardwareServerTask.UpdateState();
            }
            if (addHardwareServerTask.State == StateEnum.Error)
            {
                throw new MIPException("Hardware add error: " + addHardwareServerTask.ErrorText);
            }

            // If the creation succeeded, return the Guid of the hardware
            return new Guid(addHardwareServerTask.Path.Substring(9, 36));
        }
        private static void ConfigureHardware(Hardware hardware, string sourceCamera, string vpsServiceUrls, string gstreamerPipelineConfig)
        {
            var hwSettingsGeneral = hardware.HardwareDriverSettingsFolder.HardwareDriverSettings.First();
            var generalProperties = hwSettingsGeneral.HardwareDriverSettingsChildItems.First().Properties;

            // The source camera specifies the Guid of the original camera which the new VPS camera will pull its video from
            // The source camera will usually not be a VPS camera, but that is not a limitation.
            generalProperties.SetValue("SourceCamera", sourceCamera);

            // The configuration is a string which is sent to the VPS Service at the beginning of each processing stream.
            // You can populate it from here, or you can let the end user edit the field for the VPS camera in the XProtect management client.
            // You may also leave it empty, and configure your individual processing streams from other sources than the XProtect management client.
            generalProperties.SetValue("ServiceParameters", gstreamerPipelineConfig);

            // The parameters are defined by and used internally in the XProtect Recording Server.
            // It consists of strings separated by semicolons.
            // Here we only put in one string like "VPSNODES,URL1", and we don't need any semicolon in the end.
            generalProperties.SetValue("DriverParameters", $"VPSNODES," + vpsServiceUrls);

            hwSettingsGeneral.Save();
        }
    }
}
