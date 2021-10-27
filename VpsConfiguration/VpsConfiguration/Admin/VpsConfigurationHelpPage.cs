using System;
using System.Linq;
using System.Windows.Forms;

using VideoOS.Platform;
using VideoOS.Platform.Admin;

namespace VpsConfiguration.Admin
{
    public partial class HelpPage : ItemNodeUserControl
    {
        private Item _selectedItem = null;

        /// <summary>
        /// User control to display help page
        /// </summary>	
        public HelpPage()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Display information from or about the Item selected.
        /// </summary>
        /// <param name="item"></param>
        public override void Init(Item item)
        {
            var siteUrls = VpsCameraBuilder.GetSiteAddressesOrdered().ToArray();
            comboBoxManagementServerUrl.Items.AddRange(siteUrls);
            comboBoxManagementServerUrl.SelectedItem = siteUrls.First();
        }

        /// <summary>
        /// Close any session and release any resources used.
        /// </summary>
        public override void Close()
        {
        }

        private void buttonSource_Click(object sender, EventArgs e)
        {
            using (VideoOS.Platform.UI.ItemPickerForm form = new VideoOS.Platform.UI.ItemPickerForm())
            {
                form.Width = 550;
                form.StartPosition = FormStartPosition.CenterParent;
                form.TopMost = true;
                form.Text = "Select video source for new VPS camera(s)";
                form.TopLabel = "Selecting a group prepares for a VPS cameras for each group member";
                form.KindFilter = Kind.Camera;
                form.ValidateSelectionEvent += Form_ValidateSelectionEvent;
                form.Init(Configuration.Instance.GetItems(ItemHierarchy.UserDefined)); // .Both will include cameras which are not in any group.
                if (form.ShowDialog() == DialogResult.OK)
                {
                    _selectedItem = form.SelectedItem;
                    textBoxSource.Text = _selectedItem.Name;
                }
                form.ValidateSelectionEvent -= Form_ValidateSelectionEvent;
            }
        }

        private void Form_ValidateSelectionEvent(VideoOS.Platform.UI.ItemPickerForm.ValidateEventArgs e)
        {
            if (e.Item.FQID.Kind == Kind.Camera)
            {
                e.AcceptSelection = true;
            }
        }

        private void buttonCreate_Click(object sender, EventArgs e)
        {
            string brand = "VPS";
            try
            {
                string cameraSuffix = $"{brand} camera";
                string metadataSuffix = $"{brand} metadata";
                string cameraDeviceGroupName = $"{brand} camera group";
                string metadataDeviceGroupName = $"{brand} metadata group";
                string pipelineConfig = string.Empty;

                this.Cursor = Cursors.WaitCursor;

                if (_selectedItem.FQID.FolderType == FolderType.No)
                {
                    // The end user selected a single camera
                    string newHardwareName = $"{brand} - {_selectedItem.Name}";
                    VpsCameraBuilder.BuildOneCamera(
                        _selectedItem,
                        newHardwareName,
                        cameraDeviceGroupName,
                        cameraSuffix,
                        metadataDeviceGroupName,
                        metadataSuffix,
                        (string) comboBoxManagementServerUrl.SelectedItem,
                        textBoxService.Text,
                        pipelineConfig
                        );
                }
                else
                {
                    // The end user selected a camera group
                    string newHardwareFormat = $"{brand} - ###CAMERA_NAME###";
                    VpsCameraBuilder.BuildCameraForEachChild(
                        _selectedItem,
                        newHardwareFormat,
                        cameraDeviceGroupName,
                        cameraSuffix,
                        metadataDeviceGroupName,
                        metadataSuffix,
                        (string) comboBoxManagementServerUrl.SelectedItem,
                        textBoxService.Text,
                        pipelineConfig);
                }

                MessageBox.Show($"Created new {brand} hardware OK", $"{brand} Camera Creation", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, $"{brand} Camera Creation", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        private void check_TextBox(object sender, EventArgs e)
        {
            if (textBoxSource.Text.Length != 0 &&
                textBoxService.Text.Length != 0)
            {
                buttonCreate.Enabled = true;
            }
            else
            {
                buttonCreate.Enabled = false;
            }
        }
    }
}
