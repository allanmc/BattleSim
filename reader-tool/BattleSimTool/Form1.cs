using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Data;
using System.IO;

namespace BattleSimTool
{
    public partial class Form1 : Form
    {
        private ReadAllanMCFile m_reader;
        private DataSet m_dataSet;
        private System.Windows.Forms.ColumnHeader m_ColumnHeaderTwo;
        private System.Windows.Forms.ColumnHeader m_ColumnHeaderOne;
        private System.Windows.Forms.ListView m_ListView; 
        public Form1()
        {
            InitializeComponent();
            //this.Show();
            GameStuff gameStuff = new GameStuff();

            OpenFileDialog op = new OpenFileDialog();
            op.Filter = "Battles-Data files (*.dat)|*.dat";
            string filePath;

            filePath = "C:\\Program Files\\Spring\\AI\\Skirmish\\BattleSim\\666";

            op.DefaultExt = ".dat";
            op.InitialDirectory = filePath;
            
            DialogResult res = op.ShowDialog();
            if (DialogResult.OK != res)
                return;
            filePath = op.FileName;
        

            m_reader = new ReadAllanMCFile();
            List<float> values = m_reader.GetValues(filePath);
            int current_game = 0; 
            foreach(float value in values) {
                //ListViewItem item = new ListViewItem(new string[] { value.ToString() });
                List<string> list = gameStuff.GetUnits(current_game);
                list.Insert(0, current_game.ToString());
                list.Insert(1, value.ToString());
                ListViewItem item = new ListViewItem(list.ToArray());
                if (value > 100 || value < -100 || value == 0.0)
                {
                    item.ForeColor = Color.Red;
                }
                listView1.Items.Add(item);
                current_game++;
                //this.Show();
            }
        }
    }
}
