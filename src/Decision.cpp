#include "Decision.h"
#include "Unit.h"
#include "UnitDef.h"
#include "Cheats.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

Decision::Decision(AIClasses* aiClasses)
{
	ai = aiClasses;
	gameCounter = 0;
	ai->frame = 0;
	resettingGame = false;
	frame_reset = 0;
	frame_timeout = 0;
	timeouts = 0;
	lastSpawnFrame = -1;
	group = new Group( ai );

	ai->utility->ChatMsg("Starting game #%i", gameCounter++);
}

Decision::~Decision(void)
{
	delete group;
}

///called when a unit enters the world
///@warning the unit may not be fully constructed yet
///@warning the builderID may not corrospond to a unit
void Decision::UnitCreated(int unitID, int builderID)
{

}

///Called when a unit is fully constructed
///@param unit the ID of the unit
void Decision::UnitFinished(int unit)
{
	Unit *u = Unit::GetInstance(ai->callback,unit);
	
	UnitDef * ud = u->GetDef();
	
	ai->utility->Log(ALL, MISC, "Unit finished, \"%s\", pos:%f,%f", ud->GetName(), u->GetPos().x, u->GetPos().z);
	if(ud->GetSpeed() > 0)
	{
		ai->utility->Log(ALL, MISC, "Finished with a non-building");

		if (ud->IsCommander())
		{
			ai->commander = Unit::GetInstance(ai->callback, unit);
		}
		
	} else {
		ai->utility->Log(ALL, MISC, "Finished with a building");
	}

	delete ud;
}


///called when one of our units are destoyed
void Decision::UnitDestroyed(int unit, int attacker)
{
	ai->utility->Log(ALL, MISC,  "UnitDestroyed id = %i, name = %s", unit, Unit::GetInstance(ai->callback,unit)->GetDef()->GetName() );
}


///called when an enemy have been destroyed
void Decision::EnemyDestroyed(int enemy, int attacker)
{
	ai->utility->Log(ALL, BATTLESIM, "EnemyDestroyed %d. Attacker %d.... %d", enemy, attacker);
}


void Decision::Update(int frame)
{
	//ai->utility->Log(ALL, MISC, "Currentframe: %i", frame);
	
	gameCounter = group->GetCurrentGame();
	if(frame == 1)
	{	
		ai->utility->Log(ALL, MISC,  "I am now in frame 1!" );
		ai->utility->LaterInitialization();

		clock_t cl = clock();
		lastTime = (double)cl/CLOCKS_PER_SEC;

		if (CALCULATE_AVERAGE) ai->utility->CalculateAverage();
	}

	if ( (ai->callback->GetTeamId()== 0 && (frame == 1 || (frame>0 && frame_reset!=0)) && frame>=frame_reset && ai->utility->UnitsLeft(0)==1 && ai->utility->UnitsLeft(1)==1)
		||
		(ai->callback->GetTeamId()== 1 && ai->AIs->find(0)->second->HasSpawned()) )
	{
		ai->utility->ChatMsg("Checking if we should stop playing: %d, %d", ((group->GetCurrentGame()+2) % GAMES_BEFORE_RESTART) == 0, group->GetCurrentGame() >= TOTAL_NUMBER_OF_GAMES-1);
		if(frame != 1 && (((group->GetCurrentGame()+2) % GAMES_BEFORE_RESTART) == 0 || group->GetCurrentGame() >= TOTAL_NUMBER_OF_GAMES-1 )) //requires 125 runs
		{
			if (group->GetCurrentGame() >= TOTAL_NUMBER_OF_GAMES-1)
			{
				ai->utility->Log(ALL, BATTLESIM, "We have finished with the simulations! I no longer have a purpose...");
				group->SetCurrentGame(-1);
			}
			ai->utility->ChatMsg("Game #%d", group->GetCurrentGame());
			group->SaveCurrentGame();
			ai->utility->SuicideAllUnits();
		}
		else
		{
			group->GiveNewUnits();
			lastSpawnFrame = frame;
			ai->hasSpawned = true;

			ai->utility->Log( ALL, BATTLESIM, "Spawned + moved new units. Game %d", group->GetCurrentGame() );

			if ( group->GetCurrentGame() % 100 == 0 )
			{
				clock_t cl = clock();
				double newLastTime = (double)cl/CLOCKS_PER_SEC;
				if ( ai->callback->GetTeamId() == 0 )
				{
					ai->utility->ChatMsg( "Last 100 games: %f secs. %d timeouts", (double)newLastTime-lastTime, timeouts );
				}
				lastTime = newLastTime;
			}

		}
		if (ai->callback->GetTeamId() == 1)
		{
			ai->AIs->find(0)->second->ResetSpawn();
		}
		resettingGame = false;
		frame_reset = 0;
		frame_timeout = frame+BATTLESIM_TIMEOUT;
	}
	
	if (ai->callback->GetTeamId() == 0) {
		if (frame_timeout>0 && !resettingGame && frame > frame_timeout) {
			frame_reset = ai->frame+BATTLESIM_RESETTIME;
			group->SendPostGame();
			ai->utility->SuicideNCUnits();
			ai->utility->Log( ALL, BATTLESIM, "Timeout->reset. Game: %d", group->GetCurrentGame() );
			resettingGame = true;
			frame_timeout = 0;
			timeouts++;
		}

		if (frame%120+1 == 30 && frame != lastSpawnFrame ) {
			if (!resettingGame && (ai->utility->UnitsLeft(0)==1 || ai->utility->UnitsLeft(1)==1)) {
				frame_reset = ai->frame+BATTLESIM_RESETTIME;
				group->SendPostGame();
				ai->utility->SuicideNCUnits();
				ai->utility->Log(ALL, BATTLESIM, "Somebody won->reset. Game: %d (%d vs %d)", group->GetCurrentGame(), ai->utility->UnitsLeft(0), ai->utility->UnitsLeft(1) );
				resettingGame = true;
				frame_timeout = 0;
			}
		}
	}

	if ( (frame != 0 && frame % 300 == 1) || frame == lastSpawnFrame )
	{
		vector<Unit*> u;
		vector<Unit*> u_attack;
		for ( unsigned int i = 0 ; i < group->newUnits.size() ; i++ )
		{
			UnitDef* d = group->newUnits[i]->GetDef();
			if ( group->newUnits[i]->GetHealth() == 0.0f )
			{
				continue;
			}
			if ( strcmp(d->GetName(), "armthund") == 0 )
			{
				u_attack.push_back( group->newUnits[i] );
			}
			else
			{
				u.push_back( group->newUnits[i] );
			}
			delete d;
		}
		ai->utility->MoveUnits(u, ai->utility->GetSafePosition());
		ai->utility->AttackArea(u_attack, ai->utility->GetSafePosition());
	}
}

void Decision::UnitIdle( int id )
{
	
}


void Decision::UnitDamaged( int unitID, int attacker )
{
	
}

void Decision::EnemyDamaged( int attacker, int enemy )
{
	
}