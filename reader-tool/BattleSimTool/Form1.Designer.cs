namespace BattleSimTool
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
            this.listView1 = new System.Windows.Forms.ListView();
            this.Reward = new System.Windows.Forms.ColumnHeader();
            this.Friendly1 = new System.Windows.Forms.ColumnHeader();
            this.Friendly2 = new System.Windows.Forms.ColumnHeader();
            this.Friendly3 = new System.Windows.Forms.ColumnHeader();
            this.Enemy1 = new System.Windows.Forms.ColumnHeader();
            this.Enemy2 = new System.Windows.Forms.ColumnHeader();
            this.Enemy3 = new System.Windows.Forms.ColumnHeader();
            this.ID = new System.Windows.Forms.ColumnHeader();
            this.SuspendLayout();
            // 
            // listView1
            // 
            this.listView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.ID,
            this.Reward,
            this.Friendly1,
            this.Friendly2,
            this.Friendly3,
            this.Enemy1,
            this.Enemy2,
            this.Enemy3});
            this.listView1.GridLines = true;
            this.listView1.Location = new System.Drawing.Point(-1, -3);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(923, 354);
            this.listView1.TabIndex = 15;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Details;
            // 
            // Reward
            // 
            this.Reward.Text = "Reward";
            this.Reward.Width = 120;
            // 
            // Friendly1
            // 
            this.Friendly1.Text = "Friendly1";
            this.Friendly1.Width = 120;
            // 
            // Friendly2
            // 
            this.Friendly2.Text = "Friendly2";
            this.Friendly2.Width = 120;
            // 
            // Friendly3
            // 
            this.Friendly3.Text = "Friendly3";
            this.Friendly3.Width = 120;
            // 
            // Enemy1
            // 
            this.Enemy1.Text = "Enemy1";
            this.Enemy1.Width = 120;
            // 
            // Enemy2
            // 
            this.Enemy2.Text = "Enemy2";
            this.Enemy2.Width = 120;
            // 
            // Enemy3
            // 
            this.Enemy3.Text = "Enemy3";
            this.Enemy3.Width = 120;
            // 
            // ID
            // 
            this.ID.Text = "ID";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(922, 351);
            this.Controls.Add(this.listView1);
            this.MaximumSize = new System.Drawing.Size(930, 800);
            this.MinimumSize = new System.Drawing.Size(930, 1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.ColumnHeader Reward;
        private System.Windows.Forms.ColumnHeader Friendly1;
        private System.Windows.Forms.ColumnHeader Friendly2;
        private System.Windows.Forms.ColumnHeader Friendly3;
        private System.Windows.Forms.ColumnHeader Enemy1;
        private System.Windows.Forms.ColumnHeader Enemy2;
        private System.Windows.Forms.ColumnHeader Enemy3;
        private System.Windows.Forms.ColumnHeader ID;
    }
}

