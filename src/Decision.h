#ifndef _BRAINSPACE_DECISION_H
#define _BRAINSPACE_DECISION_H
#include "global.h"
#include "Group.h"

using namespace springai;

namespace brainSpace {
///This class has the responsibillty to choose the apropriate actions, when an event occurs.
class Decision
{
public:
	
	void UnitCreated(int unit, int builder);
	void UnitFinished(int unit); 
	void UnitDestroyed(int unit, int attacker);

	void UnitDamaged(int unit, int attacker);
	void EnemyDamaged( int attacker, int enemy );

	void EnemyDestroyed(int enemy, int attacker);
	void Update(int frame);
	void UnitIdle( int unitID );
	Decision(AIClasses* aiClasses);
	~Decision(void);

	int gameCounter;	

private:
	
	Group* group;
	
	AIClasses* ai;
	int frame_reset;
	int frame_timeout;
	
	bool resettingGame;
	double lastTime;
	int timeouts;
	int lastSpawnFrame;
};

}

#endif // _BRAINSPACE_DECISION_H
