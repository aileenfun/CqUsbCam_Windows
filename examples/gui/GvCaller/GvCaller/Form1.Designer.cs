namespace GvCaller
{
    partial class Form1
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.button2 = new System.Windows.Forms.Button();
            this.tb_expo = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.button3 = new System.Windows.Forms.Button();
            this.cb_expoauto = new System.Windows.Forms.CheckBox();
            this.label7 = new System.Windows.Forms.Label();
            this.SavePicButton = new System.Windows.Forms.Button();
            this.StopButton = new System.Windows.Forms.Button();
            this.TestButton = new System.Windows.Forms.Button();
            this.FrameRateLabel = new System.Windows.Forms.Label();
            this.pictureBox2 = new System.Windows.Forms.PictureBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.btnWrEE = new System.Windows.Forms.Button();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.btnRdEE = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.radioButton4 = new System.Windows.Forms.RadioButton();
            this.radioButton3 = new System.Windows.Forms.RadioButton();
            this.radioButton2 = new System.Windows.Forms.RadioButton();
            this.radioButton1 = new System.Windows.Forms.RadioButton();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // pictureBox1
            // 
            this.pictureBox1.Location = new System.Drawing.Point(1, 5);
            this.pictureBox1.Margin = new System.Windows.Forms.Padding(4);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(632, 605);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox1.TabIndex = 1;
            this.pictureBox1.TabStop = false;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(1486, 277);
            this.button2.Margin = new System.Windows.Forms.Padding(4);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(100, 29);
            this.button2.TabIndex = 2;
            this.button2.Text = "start";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.start_Click);
            // 
            // tb_expo
            // 
            this.tb_expo.Location = new System.Drawing.Point(1514, 365);
            this.tb_expo.Margin = new System.Windows.Forms.Padding(4);
            this.tb_expo.Name = "tb_expo";
            this.tb_expo.Size = new System.Drawing.Size(132, 25);
            this.tb_expo.TabIndex = 3;
            this.tb_expo.Text = "200";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(1452, 369);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(39, 15);
            this.label1.TabIndex = 8;
            this.label1.Text = "Expo";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(1407, 318);
            this.label6.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(167, 15);
            this.label6.TabIndex = 13;
            this.label6.Text = "(ROI在开启后不可设置)";
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(1548, 400);
            this.button3.Margin = new System.Windows.Forms.Padding(4);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(100, 29);
            this.button3.TabIndex = 15;
            this.button3.Text = "setExpo";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.SetExpo_Click);
            // 
            // cb_expoauto
            // 
            this.cb_expoauto.AutoSize = true;
            this.cb_expoauto.Checked = true;
            this.cb_expoauto.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cb_expoauto.Location = new System.Drawing.Point(1454, 409);
            this.cb_expoauto.Margin = new System.Windows.Forms.Padding(4);
            this.cb_expoauto.Name = "cb_expoauto";
            this.cb_expoauto.Size = new System.Drawing.Size(61, 19);
            this.cb_expoauto.TabIndex = 17;
            this.cb_expoauto.Text = "auto";
            this.cb_expoauto.UseVisualStyleBackColor = true;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(1407, 290);
            this.label7.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(67, 15);
            this.label7.TabIndex = 18;
            this.label7.Text = "开始采集";
            // 
            // SavePicButton
            // 
            this.SavePicButton.Location = new System.Drawing.Point(1454, 494);
            this.SavePicButton.Margin = new System.Windows.Forms.Padding(4);
            this.SavePicButton.Name = "SavePicButton";
            this.SavePicButton.Size = new System.Drawing.Size(100, 29);
            this.SavePicButton.TabIndex = 19;
            this.SavePicButton.Text = "SavePic";
            this.SavePicButton.UseVisualStyleBackColor = true;
            this.SavePicButton.Click += new System.EventHandler(this.SavePicButton_Click);
            // 
            // StopButton
            // 
            this.StopButton.Location = new System.Drawing.Point(1454, 451);
            this.StopButton.Margin = new System.Windows.Forms.Padding(4);
            this.StopButton.Name = "StopButton";
            this.StopButton.Size = new System.Drawing.Size(100, 29);
            this.StopButton.TabIndex = 20;
            this.StopButton.Text = "Stop";
            this.StopButton.UseVisualStyleBackColor = true;
            this.StopButton.Click += new System.EventHandler(this.StopButton_Click);
            // 
            // TestButton
            // 
            this.TestButton.Location = new System.Drawing.Point(1426, 13);
            this.TestButton.Margin = new System.Windows.Forms.Padding(4);
            this.TestButton.Name = "TestButton";
            this.TestButton.Size = new System.Drawing.Size(100, 29);
            this.TestButton.TabIndex = 21;
            this.TestButton.Text = "openUsb";
            this.TestButton.UseVisualStyleBackColor = true;
            this.TestButton.Click += new System.EventHandler(this.TestButton_Click);
            // 
            // FrameRateLabel
            // 
            this.FrameRateLabel.AutoSize = true;
            this.FrameRateLabel.Location = new System.Drawing.Point(1452, 258);
            this.FrameRateLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.FrameRateLabel.Name = "FrameRateLabel";
            this.FrameRateLabel.Size = new System.Drawing.Size(87, 15);
            this.FrameRateLabel.TabIndex = 22;
            this.FrameRateLabel.Text = "FrameRate:";
            // 
            // pictureBox2
            // 
            this.pictureBox2.Location = new System.Drawing.Point(753, 5);
            this.pictureBox2.Margin = new System.Windows.Forms.Padding(4);
            this.pictureBox2.Name = "pictureBox2";
            this.pictureBox2.Size = new System.Drawing.Size(632, 605);
            this.pictureBox2.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox2.TabIndex = 23;
            this.pictureBox2.TabStop = false;
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(1454, 551);
            this.textBox1.Margin = new System.Windows.Forms.Padding(4);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(132, 25);
            this.textBox1.TabIndex = 24;
            this.textBox1.Text = "1";
            // 
            // btnWrEE
            // 
            this.btnWrEE.Location = new System.Drawing.Point(1454, 595);
            this.btnWrEE.Margin = new System.Windows.Forms.Padding(4);
            this.btnWrEE.Name = "btnWrEE";
            this.btnWrEE.Size = new System.Drawing.Size(100, 29);
            this.btnWrEE.TabIndex = 25;
            this.btnWrEE.Text = "write ee";
            this.btnWrEE.UseVisualStyleBackColor = true;
            this.btnWrEE.Click += new System.EventHandler(this.btnWrEE_Click);
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(1606, 551);
            this.textBox2.Margin = new System.Windows.Forms.Padding(4);
            this.textBox2.Name = "textBox2";
            this.textBox2.Size = new System.Drawing.Size(132, 25);
            this.textBox2.TabIndex = 26;
            this.textBox2.Text = "12345";
            // 
            // btnRdEE
            // 
            this.btnRdEE.Location = new System.Drawing.Point(1606, 595);
            this.btnRdEE.Margin = new System.Windows.Forms.Padding(4);
            this.btnRdEE.Name = "btnRdEE";
            this.btnRdEE.Size = new System.Drawing.Size(100, 29);
            this.btnRdEE.TabIndex = 27;
            this.btnRdEE.Text = "read ee";
            this.btnRdEE.UseVisualStyleBackColor = true;
            this.btnRdEE.Click += new System.EventHandler(this.btnRdEE_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.radioButton4);
            this.groupBox1.Controls.Add(this.radioButton3);
            this.groupBox1.Controls.Add(this.radioButton2);
            this.groupBox1.Controls.Add(this.radioButton1);
            this.groupBox1.Location = new System.Drawing.Point(1426, 59);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(145, 133);
            this.groupBox1.TabIndex = 28;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "groupBox1";
            // 
            // radioButton4
            // 
            this.radioButton4.AutoSize = true;
            this.radioButton4.Location = new System.Drawing.Point(15, 97);
            this.radioButton4.Name = "radioButton4";
            this.radioButton4.Size = new System.Drawing.Size(84, 19);
            this.radioButton4.TabIndex = 3;
            this.radioButton4.Text = "528x390";
            this.radioButton4.UseVisualStyleBackColor = true;
            this.radioButton4.CheckedChanged += new System.EventHandler(this.radioButton4_CheckedChanged);
            // 
            // radioButton3
            // 
            this.radioButton3.AutoSize = true;
            this.radioButton3.Location = new System.Drawing.Point(15, 74);
            this.radioButton3.Name = "radioButton3";
            this.radioButton3.Size = new System.Drawing.Size(92, 19);
            this.radioButton3.TabIndex = 2;
            this.radioButton3.Text = "1052x780";
            this.radioButton3.UseVisualStyleBackColor = true;
            this.radioButton3.CheckedChanged += new System.EventHandler(this.radioButton3_CheckedChanged);
            // 
            // radioButton2
            // 
            this.radioButton2.AutoSize = true;
            this.radioButton2.Location = new System.Drawing.Point(15, 49);
            this.radioButton2.Name = "radioButton2";
            this.radioButton2.Size = new System.Drawing.Size(100, 19);
            this.radioButton2.TabIndex = 1;
            this.radioButton2.Text = "2104x1560";
            this.radioButton2.UseVisualStyleBackColor = true;
            this.radioButton2.CheckedChanged += new System.EventHandler(this.radioButton2_CheckedChanged);
            // 
            // radioButton1
            // 
            this.radioButton1.AutoSize = true;
            this.radioButton1.Checked = true;
            this.radioButton1.Location = new System.Drawing.Point(15, 24);
            this.radioButton1.Name = "radioButton1";
            this.radioButton1.Size = new System.Drawing.Size(100, 19);
            this.radioButton1.TabIndex = 0;
            this.radioButton1.TabStop = true;
            this.radioButton1.Text = "4208x3120";
            this.radioButton1.UseVisualStyleBackColor = true;
            this.radioButton1.CheckedChanged += new System.EventHandler(this.radioButton1_CheckedChanged);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1774, 655);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.btnRdEE);
            this.Controls.Add(this.textBox2);
            this.Controls.Add(this.TestButton);
            this.Controls.Add(this.btnWrEE);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.pictureBox2);
            this.Controls.Add(this.FrameRateLabel);
            this.Controls.Add(this.StopButton);
            this.Controls.Add(this.SavePicButton);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.cb_expoauto);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.tb_expo);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.pictureBox1);
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "Form1";
            this.Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.TextBox tb_expo;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.CheckBox cb_expoauto;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button SavePicButton;
        private System.Windows.Forms.Button StopButton;
        private System.Windows.Forms.Button TestButton;
        private System.Windows.Forms.Label FrameRateLabel;
        private System.Windows.Forms.PictureBox pictureBox2;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button btnWrEE;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.Button btnRdEE;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton radioButton4;
        private System.Windows.Forms.RadioButton radioButton3;
        private System.Windows.Forms.RadioButton radioButton2;
        private System.Windows.Forms.RadioButton radioButton1;
    }
}

