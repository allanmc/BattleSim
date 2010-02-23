#ifndef _BRAIN_TYPES_H
#define _BRAIN_TYPES_H

//#include <windows.h>
#include <stdarg.h>
#include <string>
#include <string.h>
#include <vector>
#include <set>
#include <queue>
#include <list>
#include <stdlib.h>
#include <map>
#include <math.h>
#include <iostream>
#include <fstream>
#include "sys/timeb.h"

#include <boost/algorithm/string.hpp>
#include "Unit.h"
#include "UnitDef.h"
#include "WeaponDef.h"
#include "WeaponMount.h"
#include "Damage.h"
#include "DataDirs.h"
#include "Engine.h"
#include "Map.h"
#include "Resource.h"
#include "AICallback.h"
#include "SSkirmishAICallback.h"
#include "OptionValues.h"
#include "Info.h"
#include "ExternalAI/Interface/AISCommands.h"
#include "Point.h"
#include "MoveData.h"
#include "Game.h"
namespace brainSpace {
enum LogTypes {
	CHAT		= 1<<0,
	QUADTREE	= 1<<1,
	DECISION	= 1<<2,
	KNOWLEDGE	= 1<<3,
	UTILITY		= 1<<4,
	EVENT		= 1<<5,
	BN			= 1<<6,
	SCOUTING	= 1<<7,
	KMEDOIDS	= 1<<8,
	GROUPING	= 1<<9,
	MISC		= 1<<10,
	SLOPEMAP	= 1<<11,
	LOG_RL		= 1<<12,
	PATHFIND	= 1<<13,
	BATTLESIM   = 1<<14
};



enum LogLevels {
	NONE		= 0,
	CRITICAL	= 1,
	IMPORTANT	= 2,
	LOG_DEBUG	= 3,
	ALL			= 4
};
}

namespace brainSpace {
	class Utility;
	class BrAIn;
	class BattleEvaluation;
	class Decision;
	//struct AIClasses;
}

namespace brainSpace {
	///A container for misc classes that all classes should know. (used due to inabillity to use Singletons)
	struct AIClasses {
		springai::AICallback *callback;
		Utility *utility;
		BattleEvaluation *be;
		springai::Unit *commander;
		int frame;
		bool hasSpawned;
		std::map<int, BrAIn*> *AIs;
		Decision* decision;
		AIClasses()
		{
			commander = NULL;
			frame = 0;
		}
	};

}
#include "BrAIn.h"
#include "BattleEvaluation.h"
#include "Utility.h"

#endif
