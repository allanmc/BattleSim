#ifndef _BRAINSPACE_GROUP_H
#define _BRAINSPACE_GROUP_H
#include "global.h"

namespace brainSpace
{
	class Group
	{
	private:
		AIClasses* ai;
		std::vector<const char*> units;
		std::vector< std::vector<const char*> > units_all;
		void RemoveCommander(std::vector<springai::Unit*> &units);
		int current_game;
		int GroupType; //0 = ground, 1 = air


	public:
		void SendPostGame();
		void SaveCurrentGame();
		int GetCurrentGame();
		void SetCurrentGame(int value);
		Group( AIClasses* aiClasses );
		~Group();
		//void GetNextBattleUnits();
		void GiveNewUnits();
		void FindNewUnits();
		std::vector<const char*> GetNewUnits(int teamId, int currentGame);

		int SumRange(int max, int min, int num);
		int GetId(int unitType, int unit1, int unit2, int unit3);

		std::vector<springai::Unit*> newUnits;

		int GetUnitId(int unitType, const char* name);
		void PrintUnitCounts(int unitType, std::vector<int> units);
		int GetNullId(int unitType);
		std::string PrintGame(int game);
		int CountGroupUnits(std::vector<springai::Unit*> units_old, std::vector<int> &result);
		int GetClosestGame();
		int* GetGroupFromUnits(std::vector<int> units);
		
	};
}
#endif
