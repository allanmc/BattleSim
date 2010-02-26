#ifndef _BRAINSPACE_BATTLE_EVALUATION_H
#define _BRAINSPACE_BATTLE_EVALUATION_H
#include "global.h"

namespace brainSpace
{
	class BattleEvaluation
	{
	private:

		void Calculate( std::vector<springai::Unit*> units, bool enemy, bool pregame = false ); //Postgame
		void Calculate( std::vector<springai::UnitDef*> unitDefs, bool enemy ); //Pregame

		AIClasses* ai;
		int BattleID;
		float EnemyStartHealth, EnemyEndHealth, FriendlyStartHealth, FriendlyEndHealth;
		float EnemyStartBuildTime, EnemyEndBuildTime, FriendlyStartBuildTime, FriendlyEndBuildTime;


		float TotalReward;

	public:
		std::vector<int> startUnits, endUnits;

		BattleEvaluation( AIClasses* aiClasses );
		virtual ~BattleEvaluation();

		void PreGame( std::vector<springai::Unit*> units, int battleID, std::vector<int> enemy );
		void PostGame( std::vector<springai::Unit*> friendly, std::vector<springai::Unit*> enemy, int battleID );

	};
}

#endif