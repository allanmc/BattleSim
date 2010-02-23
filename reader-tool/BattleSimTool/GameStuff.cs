using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BattleSimTool
{
    class GameStuff
    {
        string[][] units_all;

        public GameStuff() {
            units_all = new string[][]{new string[]{
                "armpw",
                "armrock",
                "armham",
                "armjeth",
                "armwar",
                "armflea",
                "armfav",
                "armflash",
                "armpincer",
                "armstump",
                "tawf013",
                "armjanus",
                "armsam"
            },new string[]{
                "armpeep",
                "armfig",
                "armthund",
                "armatlas",
                "armkam"
            }};
        }

        public List<string> GetUnits(int currentGame)
        {
            List<string> tmpUnits = new List<string>();

            for (int teamId = 0; teamId <= 1; teamId++ )
            {
                //magic
                int numberOfGroups = 0;
                int max;
                for (int i = 0; i < 2; i++)
                {
                    max = units_all[i].Length - 1; //id of the last unit
                    numberOfGroups += GetId(i, max, max, max) + 1;
                }
                int unitType = 0;
                max = units_all[unitType].Length - 1; //id of the last unit
                int groupId = (teamId == 0 ? currentGame % numberOfGroups : currentGame / numberOfGroups);
                int lastGroup = GetId(unitType, max, max, max);
                if (groupId > lastGroup)
                {
                    unitType = 1;
                    max = units_all[unitType].Length - 1;
                    groupId -= lastGroup + 1; // +1 to get size

                    lastGroup = GetId(unitType, max, max, max);
                    if (groupId > lastGroup)
                    {
                        //We are done!
                    }
                }
                //GroupType = unitType;
                int prevUnit = 0, tmpUnit = 0;
                int tmpId = 0;
                int block;

                max += 1; //new use of max as the number of units
                //unit1
                block = SumRange(max, 1, max);
                while (groupId >= tmpId + block)
                {
                    tmpUnit++;
                    tmpId += block;
                    block = SumRange(max - tmpUnit, 1, max - tmpUnit);
                }
                tmpUnits.Add(units_all[unitType][tmpUnit]);
                prevUnit = tmpUnit;
                tmpUnit = 0;
                //unit2
                block = max - prevUnit;
                while (groupId >= tmpId + block)
                {
                    tmpUnit++;
                    tmpId += block;
                    block--;
                }
                tmpUnit += prevUnit;
                tmpUnits.Add(units_all[unitType][tmpUnit]);
                prevUnit = tmpUnit;
                tmpUnit = 0;
                //unit3
                tmpUnit = groupId - tmpId + prevUnit;
                tmpUnits.Add(units_all[unitType][tmpUnit]);
                //end magic	
            }

            return tmpUnits;
        }

        private int SumRange(int max, int min, int num){
		    return (max + min)*num/2;
	    }

	    private int GetId(int unitType, int unit1, int unit2, int unit3) {
		    int id = 0;
		    //unit1
		    int num_units = units_all[unitType].Length;
		    int max = num_units;
		    for (int i = 0; i < unit1; i++) {
			    id += SumRange(max - i, 1, max - i);
		    }
		    //unit2
		    id += SumRange(num_units - unit1, num_units - (unit2 - unit1), unit2-unit1 );
		    //unit3
		    id += unit3 - unit2;
    		
		    return id;
	    }

    }
}
