namespace VpsConfiguration.Admin
{
    partial class HelpPage
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.labelText1 = new System.Windows.Forms.Label();
            this.labelText2 = new System.Windows.Forms.Label();
            this.labelText3 = new System.Windows.Forms.Label();
            this.labelSource = new System.Windows.Forms.Label();
            this.textBoxSource = new System.Windows.Forms.TextBox();
            this.buttonSource = new System.Windows.Forms.Button();
            this.labelText4 = new System.Windows.Forms.Label();
            this.labelService = new System.Windows.Forms.Label();
            this.textBoxService = new System.Windows.Forms.TextBox();
            this.buttonCreate = new System.Windows.Forms.Button();
            this.labelManagementServerUrl = new System.Windows.Forms.Label();
            this.comboBoxManagementServerUrl = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // labelText1
            // 
            this.labelText1.AutoSize = true;
            this.labelText1.Location = new System.Drawing.Point(43, 34);
            this.labelText1.Name = "labelText1";
            this.labelText1.Size = new System.Drawing.Size(456, 13);
            this.labelText1.TabIndex = 0;
            this.labelText1.Text = "Create VPS streams from existing source cameras by filling in parameters, then cl" +
    "icking \"Create\"";
            // 
            // labelText2
            // 
            this.labelText2.AutoSize = true;
            this.labelText2.Location = new System.Drawing.Point(56, 62);
            this.labelText2.Name = "labelText2";
            this.labelText2.Size = new System.Drawing.Size(431, 13);
            this.labelText2.TabIndex = 1;
            this.labelText2.Text = "- Each stream will have its own new \"VPS camera\", which shows up as any other cam" +
    "era.";
            // 
            // labelText3
            // 
            this.labelText3.AutoSize = true;
            this.labelText3.Location = new System.Drawing.Point(56, 90);
            this.labelText3.Name = "labelText3";
            this.labelText3.Size = new System.Drawing.Size(464, 13);
            this.labelText3.TabIndex = 2;
            this.labelText3.Text = "- The metadata from each VPS camera will be set as \"related metadata\" for each so" +
    "urce camera.";
            // 
            // labelSource
            // 
            this.labelSource.AutoSize = true;
            this.labelSource.Location = new System.Drawing.Point(43, 260);
            this.labelSource.Name = "labelSource";
            this.labelSource.Size = new System.Drawing.Size(128, 13);
            this.labelSource.TabIndex = 3;
            this.labelSource.Text = "Source camera or group:*";
            // 
            // labelManagementServerUrl
            // 
            this.labelManagementServerUrl.AutoSize = true;
            this.labelManagementServerUrl.Location = new System.Drawing.Point(43, 182);
            this.labelManagementServerUrl.Name = "labelManagementServerUrl";
            this.labelManagementServerUrl.Size = new System.Drawing.Size(122, 13);
            this.labelManagementServerUrl.TabIndex = 4;
            this.labelManagementServerUrl.Text = "Management Server Url:";

            // 
            // comboBoxManagementServerUrl
            // 
            this.comboBoxManagementServerUrl.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
            | System.Windows.Forms.AnchorStyles.Right)));
            this.comboBoxManagementServerUrl.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxManagementServerUrl.FormattingEnabled = true;
            this.comboBoxManagementServerUrl.ItemHeight = 13;
            this.comboBoxManagementServerUrl.Location = new System.Drawing.Point(46, 198);
            this.comboBoxManagementServerUrl.Name = "comboBoxManagementServerUrl";
            this.comboBoxManagementServerUrl.Size = new System.Drawing.Size(780, 21);
            this.comboBoxManagementServerUrl.TabIndex = 5;
            // 
            // textBoxSource
            // 
            this.textBoxSource.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxSource.BackColor = System.Drawing.SystemColors.Window;
            this.textBoxSource.Location = new System.Drawing.Point(46, 276);
            this.textBoxSource.Name = "textBoxSource";
            this.textBoxSource.ReadOnly = true;
            this.textBoxSource.Size = new System.Drawing.Size(780, 20);
            this.textBoxSource.TabIndex = 6;
            this.textBoxSource.TextChanged += new System.EventHandler(this.check_TextBox);
            // 
            // buttonSource
            // 
            this.buttonSource.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonSource.Location = new System.Drawing.Point(843, 275);
            this.buttonSource.Name = "buttonSource";
            this.buttonSource.Size = new System.Drawing.Size(26, 22);
            this.buttonSource.TabIndex = 7;
            this.buttonSource.Text = "...";
            this.buttonSource.UseVisualStyleBackColor = true;
            this.buttonSource.Click += new System.EventHandler(this.buttonSource_Click);
            // 
            // labelText4
            // 
            this.labelText4.AutoSize = true;
            this.labelText4.Location = new System.Drawing.Point(56, 118);
            this.labelText4.Name = "labelText4";
            this.labelText4.Size = new System.Drawing.Size(247, 13);
            this.labelText4.TabIndex = 8;
            this.labelText4.Text = "- Each stream needs a VPS Service to execute on.";
            // 
            // labelService
            // 
            this.labelService.AutoSize = true;
            this.labelService.Location = new System.Drawing.Point(43, 334);
            this.labelService.Name = "labelService";
            this.labelService.Size = new System.Drawing.Size(99, 13);
            this.labelService.TabIndex = 9;
            this.labelService.Text = "VPS Service URL:*";
            // 
            // textBoxService
            // 
            this.textBoxService.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxService.Location = new System.Drawing.Point(46, 350);
            this.textBoxService.Name = "textBoxService";
            this.textBoxService.Size = new System.Drawing.Size(780, 20);
            this.textBoxService.TabIndex = 10;
            this.textBoxService.Text = "http://localhost:5000/gstreamer/pipelines";
            this.textBoxService.TextChanged += new System.EventHandler(this.check_TextBox);
            // 
            // buttonCreate
            // 
            this.buttonCreate.Enabled = false;
            this.buttonCreate.Location = new System.Drawing.Point(46, 408);
            this.buttonCreate.Name = "buttonCreate";
            this.buttonCreate.Size = new System.Drawing.Size(118, 32);
            this.buttonCreate.TabIndex = 11;
            this.buttonCreate.Text = "Create";
            this.buttonCreate.UseVisualStyleBackColor = true;
            this.buttonCreate.Click += new System.EventHandler(this.buttonCreate_Click);
           
            // 
            // HelpPage
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.WhiteSmoke;
            this.Controls.Add(this.comboBoxManagementServerUrl);
            this.Controls.Add(this.labelManagementServerUrl);
            this.Controls.Add(this.buttonCreate);
            this.Controls.Add(this.textBoxService);
            this.Controls.Add(this.labelService);
            this.Controls.Add(this.labelText4);
            this.Controls.Add(this.buttonSource);
            this.Controls.Add(this.textBoxSource);
            this.Controls.Add(this.labelSource);
            this.Controls.Add(this.labelText3);
            this.Controls.Add(this.labelText2);
            this.Controls.Add(this.labelText1);
            this.Name = "HelpPage";
            this.Size = new System.Drawing.Size(915, 710);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label labelText1;
        private System.Windows.Forms.Label labelText2;
        private System.Windows.Forms.Label labelText3;
        private System.Windows.Forms.Label labelSource;
        private System.Windows.Forms.TextBox textBoxSource;
        private System.Windows.Forms.Button buttonSource;
        private System.Windows.Forms.Label labelText4;
        private System.Windows.Forms.Label labelService;
        private System.Windows.Forms.TextBox textBoxService;
        private System.Windows.Forms.Button buttonCreate;
        private System.Windows.Forms.Label labelManagementServerUrl;
        private System.Windows.Forms.ComboBox comboBoxManagementServerUrl;
    }
}
