using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
namespace BattleSimTool
{
    class ReadAllanMCFile
    {
        public List<float> GetValues(string filePath)
        {

            List<float> list = new List<float>();
            Stream s = File.OpenRead(filePath);
            BinaryReader reader = new BinaryReader(s);

            float val = 0.0f;
            int counter = 0;
            float max = 0;
            try
            {
                while (reader.BaseStream.Position < reader.BaseStream.Length)
                {
                    val = reader.ReadSingle();
                    list.Add(val);
                    counter++;
                    max = (val > max ? val : max);
                }
            }
            catch (Exception e)
            {
            }
            Console.WriteLine("Max value (" + counter + "): " + max);

            return list;
        }
    }
}
