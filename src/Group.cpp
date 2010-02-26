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
		//current_game = 1005;
		//current_game = TOTAL_NUMBER_OF_GAMES-20;//start -1
		ai->utility->Log( ALL, BATTLESIM, "GROUP: CURRENT GAME: %d", current_game );
		
		/*
		armpw(120),
		armrock(127),
		armham(82),
		armjeth(89)
		armwar(173),
		armfav(61),
		armflash(70),
		armpincer(116),
		armstump(151),
		tawf013(368),
		armjanus(88),
		armsam(130),

		armfig(68),
		armthund(157),
		armkam(90)

		*/

		vector<int> ground;
		vector<int> air;
		ground.push_back(120);	//0 - armpw
		ground.push_back(127);	//1 - armrock
		ground.push_back(82);	//2 - armham
		ground.push_back(89);	//3 - armjeth
		ground.push_back(173);	//4 - armwar
		ground.push_back(61);	//5 - armfav
		ground.push_back(70);	//6 - armflash
		ground.push_back(116);	//7 - armpincer
		ground.push_back(151);	//8 - armstump
		ground.push_back(368);	//9 - tawf013
		ground.push_back(88);	//10 - armjanus
		ground.push_back(130);	//11 - armsam
		ground.push_back(-1);	//12 - nullground

		//air.push_back("armpeep");
		air.push_back(-2);	//0 - nullair
		air.push_back(68);	//1 - armfig
		air.push_back(157);	//2 - armthund
		air.push_back(90);	//3 - armkam
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
			if ( units[i] < 0 ) //Null-units a negtive (-1, -2)
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


	GroupInfo Group::GameToGroup(int teamId, int currentGame)
	{
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
		
		int globalGroupId = groupId;

		if(groupId > lastGroup)
		{

			unitType = 1;
			max = units_all[unitType].size()-1;
			groupId -= lastGroup+1; // +1 to get size
			
			//lastGroup = GetId(unitType, max, max, max);
		}

		return (GroupInfo){unitType,groupId,globalGroupId};		
	}

	vector<int> Group::GetNewUnits(int teamId, int currentGame)
	{
		vector<int> tmpUnits;

		GroupInfo groupInfo = GameToGroup(teamId, currentGame);

		int max = units_all[groupInfo.unitType].size()-1; //id of the last unit

		
		//ai->utility->ChatMsg("GetNewUnits groupId: %d", groupId);

		GroupType = groupInfo.unitType;
		int prevUnit = 0, tmpUnit = 0;
		int tmpId = 0;
		int block;

		max += 1; //new use of max as the number of units
		//unit1
		block = SumRange(max, 1, max);
		while(groupInfo.groupId >= tmpId + block)
		{
			tmpUnit++;
			tmpId += block;
			block = SumRange(max - tmpUnit, 1, max - tmpUnit);
		}
		tmpUnits.push_back(units_all[groupInfo.unitType][tmpUnit]);
		prevUnit = tmpUnit;
		tmpUnit = 0;
		//unit2
		block = max - prevUnit;
		while(groupInfo.groupId >= tmpId + block)
		{
			tmpUnit++;
			tmpId += block;
			block--;
		}
		tmpUnit += prevUnit;
		tmpUnits.push_back(units_all[groupInfo.unitType][tmpUnit]);
		prevUnit = tmpUnit;
		tmpUnit = 0;
		//unit3
		tmpUnit = groupInfo.groupId - tmpId + prevUnit;
		tmpUnits.push_back(units_all[groupInfo.unitType][tmpUnit]);
		//end magic	

		return tmpUnits;
	}

	int Group::SumRange(int max, int min, int num){
		return (max + min)*num/2;
	}

	int Group::GetId(int unitType, int unit1, int unit2, int unit3) {
		int id = 0;
		
		//Fix input
		if (unit1 > unit2)
		{
			swap(unit1, unit2);
		}
		if (unit1 > unit3)
		{
			swap(unit1,unit3);
		}
		if ( unit2 > unit3 )
		{
			swap(unit2,unit3);
		}
		
		//unit1
		int num_units = units_all[unitType].size();
		int max = num_units ;
		for (int i = 0; i < unit1; i++) {
			id += SumRange(max - i, 1, max - i);
		}
		//unit2
		id += SumRange(num_units - unit1, num_units - unit2 + 1, unit2-unit1 );
		//unit3
		id += unit3 - unit2;
		
		return id;
	}

	//Return unitType, and populates `result`
	int Group::CountGroupUnits(vector<Unit*> units_old, vector<int> &result)
	{
		int ground = 0;
		int air = 0;
		int unknownUnits = 0;
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
			unknownUnits++;//Pretend we won't find the unit
			for( unsigned int j = 0 ; j < units_all[unitType].size() ; j++ )
			{
				if( ud->GetUnitDefId() == units_all[unitType][j] )
				{
					//ai->utility->ChatMsg("Added unit: (%d) %s", j, ud->GetName());
					units[unitType][j]++;
					unknownUnits--;//We found the unit, so go back to all-good :-)
					break;
				}
			}
			delete ud;
		}
		int unitType = (air > ground ? 1 : 0);
		result = units[unitType];

		if ( unknownUnits>0 )
		{
			ai->utility->Log(ALL, MISC, "CountGroupUnits encountered %d unknown units!", unknownUnits );
		}

		return unitType;
	}

	string Group::PrintGame(int game)
	{
		stringstream output;
		output << "GAME #" << game << ": ";
		for (int i = 0; i <= 1 ; i++) {
			output << "Team #" << i << " - ";
			vector<int> team = GetNewUnits(i, game);
			output << team[0] << ", ";
			output << team[1] << ", ";
			output << team[2] << "; ";
		}
		return output.str();
	}

	void Group::PrintUnitCounts(int unitType, vector<int> units) 
	{
		ai->utility->ChatMsg("Printing unit counts:");
		for( unsigned int i=0 ; i < units.size() ; i++ )
		{
			ai->utility->ChatMsg("%s: %d", units_all[unitType][i], units[i]);
		}
		ai->utility->ChatMsg(".");
	}

	int Group::GetUnitId(int unitType, int defId )
	{
		for( unsigned int j = 0 ; j < units_all[unitType].size() ; j++ )
		{
			if( units_all[unitType][j] == defId )
			{
				return j;
			}
		}
		return -1;
	}

	int Group::GetUnitId(int unitType, const char* name)
	{
		return GetUnitId(unitType, ai->utility->GetUnitDef(name)->GetUnitDefId());
	}

	int Group::GetNullId(int unitType)
	{
		for( unsigned int j = 0 ; j < units_all[unitType].size() ; j++ )
		{
			if( units_all[unitType][j] < 0 ) //Null-units are negative (-1, -2)
			{
				return j;
			}
		}
		return -1;
	}


	vector<int> Group::DetermineWhatToBuild()
	{
		GroupInfo groupInfo = GameToGroup(1, GetClosestGame());
		unsigned int start = groupInfo.globalGroupId * sqrt(TOTAL_NUMBER_OF_GAMES) + 0;
		unsigned int end = groupInfo.globalGroupId * sqrt(TOTAL_NUMBER_OF_GAMES) + sqrt(TOTAL_NUMBER_OF_GAMES);
		
		int myTeam = 0;
		int bestGame = -1;
		int bestValue = 0;
		
		for ( unsigned int i = start ; i < end ; i++ )
		{
			float goodness = ai->utility->ReadBattleValue( i, true );

			vector<int> units = GetNewUnits( myTeam, i );

			float score = goodness;
			score -= ( units[0] >= 0 ? ai->utility->GetUnitDef(units[0])->GetBuildTime()/10000 : 0 );
			score -= ( units[1] >= 0 ? ai->utility->GetUnitDef(units[1])->GetBuildTime()/10000 : 0 );
			score -= ( units[2] >= 0 ? ai->utility->GetUnitDef(units[2])->GetBuildTime()/10000 : 0 );

			ai->utility->ChatMsg("Current goodness (%d): %f, score: %f", i, goodness, score);

			if ( bestGame<0 || score>bestValue )
			{
				bestGame = i;
				bestValue = score;
			}
		}

		return GetNewUnits( myTeam, bestGame );
	}

	int Group::GetClosestGame()
	{
		vector<Unit*> friendlies = ai->callback->GetFriendlyUnits();
		RemoveCommander(friendlies);
		vector<Unit*> enemies = ai->callback->GetEnemyUnits();
		RemoveCommander(enemies);
		int num_friendlies = friendlies.size();
		int num_enemies = enemies.size();

		vector<int> friendlyUnits;
		vector<int> enemyUnits;
		int friendlyUnitType = CountGroupUnits(friendlies, friendlyUnits);
		int enemyUnitType = CountGroupUnits(enemies, enemyUnits);

		//Make the unit counts of the two teams match, by adding null-units
		int diff = num_friendlies - num_enemies;
		if ( diff > 0 )
		{
			enemyUnits[GetNullId(enemyUnitType)] = diff;
			num_enemies += diff; 
		}
		else
		{
			//Always ensure we have one null-unit, so that no group of size 0 can get a slot
			enemyUnits[GetNullId(enemyUnitType)] = 1;
		}

		if ( diff < 0 )
		{
			friendlyUnits[GetNullId(friendlyUnitType)] = -1*diff;
			num_friendlies -= diff; 
		}
		else
		{
			//Always ensure we have one null-unit, so that no group of size 0 can get a slot
			friendlyUnits[GetNullId(friendlyUnitType)] = 1;
		}	

		int *friendlyGroup = GetGroupFromUnits(friendlyUnits);
		int *enemyGroup = GetGroupFromUnits(enemyUnits);
		

		int friendlyId = GetId(friendlyUnitType, friendlyGroup[0], friendlyGroup[1], friendlyGroup[2]);
		int enemiesId = GetId(enemyUnitType, enemyGroup[0], enemyGroup[1], enemyGroup[2]);

		int game = enemiesId * sqrt(TOTAL_NUMBER_OF_GAMES) + friendlyId;
		
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
