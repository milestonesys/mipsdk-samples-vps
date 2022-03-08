using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Drawing;
using System.Reflection;
using System.Windows.Forms;
using VpsConfiguration.Admin;
using VideoOS.Platform;
using VideoOS.Platform.Admin;

namespace VpsConfiguration
{
    public class VpsConfigurationDefinition : PluginDefinition
    {
        // When you have compiled a DLL from this project, you have two new files: plugin.def and VpsConfiguration.dll
        // Go to the MIPPlugins directory of your VMS system's Management Server, the default is C:\Program Files\Milestone\MIPPlugins,
        // and create a sub-directory here. You can name it as you wish, if you did not alter the code, you may use "VpsConfiguration"
        // Copy the the two files there and restart the XProtect Management Client.
        // You may have to stop the management server before copying the files.

        // If you make your own version of this plug-in, even if it is only for branding it:
        // Please remember to change the two GUIDs below to have your own unique values.
        // Your plug-in can still have the name "VpsConfiguration", but on the VMS system, you must install it in
        // a MIPPlugins subdirectory with a name different from "VpsConfiguration", which Milestone reserves for this demo version.
        // If you follow this rule, all brandings of this plug-in can co-exist.

        internal static Guid VpsConfigurationPluginId = new Guid("1D0735E4-31FC-43EE-974B-F3AAA320D841");
        internal static Guid VpsConfigurationKind = new Guid("3E3BF60A-6711-4AC7-802E-C7DB1D6DC1CB");

        private List<ItemNode> _itemNodes = new List<ItemNode>();     // The items which define this plug-in to the "system"
        private UserControl _helpPage;                                // The UserControl which appear when you click the plug-ins top level item
        private static System.Drawing.Image _serverImage;             // The icon for the plug-in's top level item
        private static System.Drawing.Image _streamImage;             // The icon for the item under the top level

        static VpsConfigurationDefinition()
        {
            _streamImage = Properties.Resources.Stream;
            _serverImage = Properties.Resources.Server;
        }

        public override void Init()
        {
            // The top level tree item is created by the "system"
            // Create the subitem which is to be presented under it
            _itemNodes.Add(new ItemNode(VpsConfigurationKind, Guid.Empty,
                                         "", _streamImage,
                                         "", _streamImage,
                                         Category.Text, false,
                                         ItemsAllowed.One, // We don't actually want MIP items, just a UserControl where we can add UI.
                                         null, null));
        }

        public override void Close()
        {
            _itemNodes.Clear();
        }

        /// <summary>
        /// We want the plug-in to have its root item in the root of the management client's tree view.
        /// </summary>
        public override AdminPlacementHint AdminPlacementHint
        {
            get { return AdminPlacementHint.Root; }
        }

        public override Guid Id
        {
            get
            {
                return VpsConfigurationPluginId;
            }
        }

        public override string Name
        {
            get { return "Video Processing Service"; }
        }

        public override string Manufacturer
        {
            get
            {
                return "Milestone Systems A/S";
            }
        }

        public override string VersionString
        {
            get
            {
                return "1.0.1.0";
            }
        }

        /// <summary>
        /// Icon to be used on top level item
        /// </summary>
        public override System.Drawing.Image Icon
        {
            get { return _serverImage; }
        }

        /// <summary>
        /// A list of server side configuration items in the administrator. This plug-in has only 1 item in the list.
        /// </summary>
        public override List<ItemNode> ItemNodes
        {
            get { return _itemNodes; }
        }

        /// <summary>
        /// A user control to display when the administrator clicks on the top TreeNode
        /// </summary>
        public override UserControl GenerateUserControl()
        {
            _helpPage = new HelpPage();
            return _helpPage;
        }

        /// <summary>
        /// This property can be set to true, to be able to display your own help UserControl on the entire panel.
        /// When this is false - a standard top and left side is added by the system.
        /// </summary>
        public override bool UserControlFillEntirePanel
        {
            get { return true; }
        }
    }
}
