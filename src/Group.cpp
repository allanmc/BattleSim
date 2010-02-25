#include "Group.h"
#include <sstream>

using namespace std;
using namespace springai;
using namespace brainSpace;

namespace brainSpace
{
	Group::Group( AIClasses* aiClasses )
	{
		ai = aiClasses;
		
		ai->utility->Log(ALL, MISC, "Constructing Group");
		current_game = ai->utility->GetCurrentGameNumber();
		current_game = 14;
		//current_game = TOTAL_NUMBER_OF_GAMES-20;//start -1
		ai->utility->Log( ALL, BATTLESIM, "GROUP: CURRENT GAME: %d", current_game );
		
		vector<const char*> ground;
		vector<const char*> air;
		ground.push_back("armpw");		//0
		ground.push_back("armrock");	//1
		ground.push_back("armham");		//2
		ground.push_back("armjeth");	//3
		ground.push_back("armwar");		//4
		//ground.push_back("armflea");
		ground.push_back("armfav");		//5
		ground.push_back("armflash");	//6
		ground.push_back("armpincer");	//7
		ground.push_back("armstump");	//8
		ground.push_back("tawf013");	//9
		ground.push_back("armjanus");	//10
		ground.push_back("armsam");		//11
		ground.push_back("nullground");	//12

		//air.push_back("armpeep");
		air.push_back("nullair");
		air.push_back("armfig");
		air.push_back("armthund");
		//air.push_back("armatlas");
		air.push_back("armkam");
		units_all.push_back(ground);
		units_all.push_back(air);

		ai->utility->Log(ALL, MISC, "Done Constructing Group");
	}
	Group::~Group()
	{
		
	}
	
	void Group::SaveCurrentGame()
	{	
		DataDirs* dirs = DataDirs::GetInstance( ai->callback );
		fstream* hest = new fstream( (string(dirs->GetWriteableDir())+"hest.dat").c_str() , ios::binary | ios::out);
		hest->write((char*)&current_game, sizeof(current_game));
		hest->close();
		delete dirs;
		delete hest;
	}

	void Group::RemoveCommander(vector<Unit*> &units)
	{
		for ( unsigned int i = 0 ; i < units.size() ; i++ )
		{
			UnitDef *d = units[i]->GetDef();
			if(d->IsCommander())
			{
				delete units[i];
				units.erase(units.begin() + i);
				delete d;
				break;
			}
			delete d;
		}
	}

	void Group::SendPostGame()
	{
		int enemyBattleID	= ai->AIs->find( ai->callback->GetTeamId() == 0 ? 1 :0 )->second->GetBattleID();
		if ( enemyBattleID !=  current_game )
		{
			ai->utility->ChatMsg("BATTLEID-ERROR: Enemy BattleID: %d. My BattleID: %d", enemyBattleID, current_game );
		}
		
		vector<Unit*> friendlyUnits = ai->callback->GetFriendlyUnits();
		RemoveCommander(friendlyUnits);
		
		vector<Unit*> enemyUnits = ai->callback->GetEnemyUnits();
		RemoveCommander(enemyUnits);

		if (ai->callback->GetTeamId() != 0)
		{
			return;
		}
		ai->be->PostGame(friendlyUnits, enemyUnits, current_game);
		ai->utility->DeleteUnits(friendlyUnits);
		ai->utility->DeleteUnits(enemyUnits);
	}

	void Group::GiveNewUnits()
	{
		FindNewUnits();

		ai->utility->Log( ALL, BATTLESIM, "Spawning new units for game: %d", current_game );
		newUnits.clear();
		
		for ( unsigned int i = 0 ; i < units.size() ; i++ )
		{
			if ( (strcmp( units[i], "nullground" ) == 0 ) || ( strcmp( units[i], "nullair" ) == 0 ) )
			{
				//Skipping null-units
				continue;
			}
			for (int c = 0; c < 4 ; c++)
			{
				newUnits.push_back(ai->utility->GiveUnit(units[i]));
			}
		}
		vector<Unit*> testUnits = ai->callback->GetFriendlyUnits();
		//ai->utility->ChatMsg( "Spawned %d new units for game: %d, check = %d", newUnits.size(), current_game, testUnits.size() );
		ai->utility->DeleteUnits(testUnits);

		if ( ai->callback->GetTeamId() == 0 ) {
			ai->be->PreGame(newUnits, current_game, GetNewUnits(1, current_game ) );
		}
	}

	void Group::FindNewUnits()
	{
		current_game++;
		units.clear();
		units = GetNewUnits(ai->callback->GetTeamId(), current_game);
	}

	void Group::SetCurrentGame(int value)
	{
		current_game = value;
	}


	int Group::GetCurrentGame()
	{
		return current_game;
	}

	vector<const char*> Group::GetNewUnits(int teamId, int currentGame)
	{
		vector<const char*> tmpUnits;

		//magic
		int numberOfGroups = 0;
		for(int i = 0; i<2; i++)
		{
			int max = units_all[i].size()-1; //id of the last unit
			numberOfGroups += GetId(i, max, max, max) + 1;
		}
		int unitType = 0;
		int max = units_all[unitType].size()-1; //id of the last unit
		int groupId = (teamId == 0 ? currentGame % numberOfGroups : currentGame / numberOfGroups );
		int lastGroup = GetId(unitType, max, max, max);
		
		if(groupId > lastGroup)
		{

			unitType = 1;
			max = units_all[unitType].size()-1;
			groupId -= lastGroup+1; // +1 to get size
			
			lastGroup = GetId(unitType, max, max, max);
		}
	
		GroupType = unitType;
		int prevUnit = 0, tmpUnit = 0;
		int tmpId = 0;
		int block;

		max += 1; //new use of max as the number of units
		//unit1
		block = SumRange(max, 1, max);
		while(groupId >= tmpId + block)
		{
			tmpUnit++;
			tmpId += block;
			block = SumRange(max - tmpUnit, 1, max - tmpUnit);
		}
		tmpUnits.push_back(units_all[unitType][tmpUnit]);
		prevUnit = tmpUnit;
		tmpUnit = 0;
		//unit2
		block = max - prevUnit;
		while(groupId >= tmpId + block)
		{
			tmpUnit++;
			tmpId += block;
			block--;
		}
		tmpUnit += prevUnit;
		tmpUnits.push_back(units_all[unitType][tmpUnit]);
		prevUnit = tmpUnit;
		tmpUnit = 0;
		//unit3
		tmpUnit = groupId - tmpId + prevUnit;
		tmpUnits.push_back(units_all[unitType][tmpUnit]);
		//end magic	

		return tmpUnits;
	}

	int Group::SumRange(int max, int min, int num){
		return (max + min)*(num/2);
	}

	int Group::GetId(int unitType, int unit1, int unit2, int unit3) {
		int id = 0;
		//unit1
		int num_units = units_all[unitType].size();
		int max = num_units ;
		for (int i = 0; i < unit1; i++) {
			id += SumRange(max - i, 1, max - i);
		}
		//unit2
		id += SumRange(num_units - unit1, num_units - (unit2 - unit1), unit2-unit1 );
		//unit3
		id += unit3 - unit2;
		
		return id;
	}

	//Return unitType, and populates `result`
	int Group::CountGroupUnits(vector<Unit*> units_old, vector<int> &result)
	{
		int ground = 0;
		int air = 0;
		//init vectors
		vector<vector<int> > units;
		for( unsigned int i = 0 ; i < units_all.size() ; i++ )
		{
			vector<int> vec;
			for( unsigned int j=0 ; j < units_all[i].size() ; j++ )
			{
				vec.push_back(0);
			}
			units.push_back(vec);
		}
		
		//fill vectors
		for( unsigned int i=0 ; i < units_old.size() ; i++ )
		{
			UnitDef* ud = units_old[i]->GetDef();
			int unitType;
			if(ud->IsAbleToFly())
			{
				unitType = 1;
				air++;
			}
			else
			{
				unitType = 0;
				ground++;
			}
			for( unsigned int j = 0 ; j < units_all[unitType].size() ; j++ )
			{
				if( strcmp(ud->GetName(), units_all[unitType][j]) == 0 )
				{
					//ai->utility->ChatMsg("Added unit: (%d) %s", j, ud->GetName());
					units[unitType][j]++;
					break;
				}
			}
			delete ud;
		}
		int unitType = (air > ground ? 1 : 0);
		result = units[unitType];

		return unitType;
	}

	string Group::PrintGame(int game)
	{
		stringstream output;
		output << "GAME #" << game << ": ";
		for (int i = 0; i <= 1 ; i++) {
			output << "Team #" << i << " - ";
			vector<const char*> team = GetNewUnits(i, game);
			output << team[0] << ", ";
			output << team[1] << ", ";
			output << team[2] << "; ";
		}
		return output.str();
	}

	int Group::GetClosestGame()
	{
		vector<Unit*> friendlies = ai->callback->GetFriendlyUnits();
		vector<Unit*> enemies = ai->callback->GetEnemyUnits();
		
		vector<int> units;
		int friendlyUnitType = CountGroupUnits(friendlies, units);
		int *friendlyGroup = GetGroupFromUnits(units);
		units.clear();
		int enemyUnitType = CountGroupUnits(enemies, units);
		int *enemyGroup = GetGroupFromUnits(units);
		
		int friendlyId = GetId(friendlyUnitType, friendlyGroup[0], friendlyGroup[1], friendlyGroup[2]);
		int enemiesId = GetId(enemyUnitType, enemyGroup[0], enemyGroup[1], enemyGroup[2]);
		
		//Bug test START

		ai->utility->ChatMsg("Test #1: %s", PrintGame(GetId(0, 0, 0, 1)).c_str());
		ai->utility->ChatMsg("Test #2: %s", PrintGame(GetId(0, 0, 0, 10)).c_str());
		ai->utility->ChatMsg("Test #3: %s", PrintGame(GetId(0, 0, 0, 11)).c_str());
		ai->utility->ChatMsg("Test #4: %s", PrintGame(GetId(0, 0, 0, 12)).c_str());
		ai->utility->ChatMsg("Test #5: %s", PrintGame(GetId(0, 0, 1, 0)).c_str());
		ai->utility->ChatMsg("Test #6: %s", PrintGame(GetId(0, 1, 0, 0)).c_str());

		ai->utility->ChatMsg("Got friendly team (%d) consisting of type %d units: %d, %d and %d", friendlyId, friendlyUnitType, friendlyGroup[0], friendlyGroup[1], friendlyGroup[2]);

		ai->utility->ChatMsg("Got enemy team (%d) consisting of type %d units: %d, %d and %d", enemiesId, enemyUnitType, enemyGroup[0], enemyGroup[1], enemyGroup[2]);
		//Bug test END


		int game = friendlyId * sqrt(TOTAL_NUMBER_OF_GAMES) + enemiesId;
		
		ai->utility->DeleteUnits(friendlies);
		ai->utility->DeleteUnits(enemies);
		delete[] friendlyGroup;
		delete[] enemyGroup;

		return game;
	}

	int* Group::GetGroupFromUnits(vector<int> units)
	{
		list<pair<int, int> > best;
		list<pair<int, int> >::iterator it;
		
		for( unsigned int i=0 ; i < units.size() ; i++ )
		{
			bool inserted = false;
			it = best.begin();
			while( it != best.end() )
			{
				if( (*it).second < units[i] )
				{
					best.insert(it, pair<int, int>(i, units[i]));
					inserted = true;
					break;
				}
				it++;
			}
			if( !inserted )
			{
				best.push_back(pair<int,int>(i, units[i]));
			}
		}
		
		int *outUnits = new int[3];
		it = best.begin();
		int size = (*it).second + (*(++it)).second + (*(++it)).second;
		it = best.begin();
		outUnits[0] = (*it).first;
		//ai->utility->ChatMsg("Group #1 set to: %d, count = %d", outUnits[0], (*(it)).second);
		float ratio = (float)(*it).second / (float)size;
		//ai->utility->ChatMsg("Ratio: %f", ratio);
		if( ratio > 0.5 )
		{
			outUnits[1] = (*it).first;
			//ai->utility->ChatMsg("Group #2 set to: %d, count = %d", outUnits[1], (*(it)).second);
			if( ratio > 0.8333 )
			{
				outUnits[2] = (*it).first;
			}
			else
			{
				outUnits[2] = (*(++it)).first;
			}
		}
		else
		{
			
			outUnits[1] = (*(++it)).first;
			//ai->utility->ChatMsg("Group #2 set to: %d, count = %d", outUnits[1], (*(it)).second);
			outUnits[2] = (*(++it)).first;
		}
		//ai->utility->ChatMsg("Group #3 set to: %d, count = %d", outUnits[2], (*(it)).second);

		best.clear();

		return outUnits;
	}

}
