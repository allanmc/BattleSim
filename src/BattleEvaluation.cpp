#include "Group.h"

#include <sstream>
using namespace std;
using namespace springai;
using namespace brainSpace;

namespace brainSpace
{
	BattleEvaluation::BattleEvaluation( AIClasses* aiClasses )
	{
		ai = aiClasses;
	}

	BattleEvaluation::~BattleEvaluation()
	{
	}


	/**PREGAME**/
	void BattleEvaluation::Calculate( std::vector<UnitDef*> unitDefs, bool enemy )
	{
		float health = 0.0f;
		float buildtime = 0.0f;


		for ( unsigned int i = 0 ; i < unitDefs.size() ; i++ )
		{
			health += unitDefs[i]->GetHealth();
			buildtime += unitDefs[i]->GetBuildTime();
		}
		health *= 4;
		buildtime *= 4;

		if ( enemy )
		{
			EnemyStartHealth = health;
			EnemyStartBuildTime = buildtime;
		}
	}

	void BattleEvaluation::Calculate( std::vector<springai::Unit*> units, bool enemy, bool pregame )
	{
		float health = 0.0f;
		float buildtime = 0.0f;
		
		for ( unsigned int i = 0 ; i < units.size() ; i++ )
		{
			UnitDef* d = units[i]->GetDef();

			float h = units[i]->GetHealth();
			if ( h > d->GetHealth() )
			{
				h = d->GetHealth();
			}
			health += h;
			int hax = d->GetBuildTime()*(h/d->GetHealth());
			buildtime += hax;
			delete d;
		}
		if ( enemy )
		{
			EnemyEndHealth = health;
			EnemyEndBuildTime = (buildtime/EnemyStartBuildTime)*100;
		}
		else
		{
			if ( !pregame )
			{
				FriendlyEndHealth = health;
				FriendlyEndBuildTime = (buildtime/FriendlyStartBuildTime)*100;
			}
			else
			{
				FriendlyStartBuildTime = buildtime;
				FriendlyStartHealth = health;
			}
		}
	}

	void BattleEvaluation::PreGame( std::vector<Unit*> units, int battleID, std::vector<int> enemy )
	{
		vector<UnitDef*> eDefs;
		
		for ( unsigned int i = 0 ; i < enemy.size() ; i++ )
		{
			if ( enemy[i] < 0 ) //Null-units are negative (-1, -2)
				continue;
			eDefs.push_back( ai->utility->GetUnitDef( enemy[i] ));
		}

		Calculate( eDefs, true );
		Calculate( units, false, true );
		BattleID = battleID;
	}

	void BattleEvaluation::PostGame( std::vector<Unit*> friendly, std::vector<Unit*> enemy, int battleID )
	{
		if ( BattleID == battleID )
		{
			Calculate( friendly, false );
			Calculate( enemy, true );

			if ( EnemyEndHealth == 0.0f )
			{
				TotalReward = FriendlyEndBuildTime;
			}
			else
			{
				if ( !(FriendlyEndHealth == 0.0f ) )
				{
					//Both sides have stuff left
					TotalReward = FriendlyEndBuildTime - EnemyEndBuildTime;
				}
				else
				{
					//Enemy won
					TotalReward = -EnemyEndBuildTime;
				}
			}
			//ai->utility->ChatMsg("Reward: %f", TotalReward );
			ai->utility->WriteBattleValue(BattleID, TotalReward);
		}

		if ( (BattleID % 100 ) == 0 )
		{
			ai->utility->ChatMsg( "%d matches played (%f%)", BattleID, (float)(BattleID/(float)TOTAL_NUMBER_OF_GAMES*100));
		}
	}
}