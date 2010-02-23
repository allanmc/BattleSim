#ifndef _BRAINSPACE_BRAIN_H
#define _BRAINSPACE_BRAIN_H

//#include "AICallback.h"
//#include <string>
#include "ExternalAI/Interface/AISEvents.h"
//#include "ExternalAI/Interface/AISCommands.h"
//#include "Unit.h"
//#include "UnitDef.h"
//#include "Engine.h"

#include "Decision.h"

namespace brainSpace {


///The brAIn!
/**
 * This is the main C++ entry point of this AI.
 * 
 * @author	Group d513a, at Aalborg University (d513a@cs.aau.dk)
 */

class BrAIn {

private:
	int teamId;

	AIClasses *ai;
	Decision *decision;

public:
	BrAIn(springai::AICallback* callback, std::map<int, brainSpace::BrAIn*> *AIs);
	~BrAIn();

	int HandleEvent(int topic, const void* data);
	int GetBattleID();
	bool HasSpawned();
	void ResetSpawn();

}; // class BrAIn

} // namespace brainSpace

#endif // _BRAINSPACE_BRAIN_H

