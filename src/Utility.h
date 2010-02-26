#ifndef _BRAINSPACE_UTILITY_H
#define _BRAINSPACE_UTILITY_H
#define MAGIC_CIRCLE_NUMBER 0.55228475
#include "types.h"
#include "Teams.h"
#include "Cheats.h"

#define BATTLES_HEADER_SIZE 0

const SAIFloat3 TEAM_0_COLOR = { 255, 0, 0 };
const SAIFloat3 TEAM_1_COLOR = { 255, 255, 0 };
const SAIFloat3 TEAM_2_COLOR = { 255, 0, 255 };
const SAIFloat3 TEAM_3_COLOR = { 0, 0, 255 };
const SAIFloat3 TEAM_4_COLOR = { 0, 255, 255 };
const SAIFloat3 TEAM_5_COLOR = { 0, 255, 0 };
const SAIFloat3 TEAM_6_COLOR = { 255, 255, 255 };
const SAIFloat3 TEAM_7_COLOR = { 0, 0, 0 };

namespace brainSpace
{
	
	///A collection of functions that provide shortcuts to commonly used functionallity
	class Utility
	{
	private:
		AIClasses* ai;
		FILE *fp;
		
		std::string battles_file;
		std::string avarage_file;
		std::fstream *bfs;
		//std::streampos battles_pointer;
		void AssignColorToGraphics( int figureGroupID );
		bool debug;
		void InitializeOptions();
		springai::UnitDef *mexDef;
		springai::UnitDef *solarDef;
		springai::UnitDef *lltDef;
		SAIFloat3 safePosition;
		std::vector<springai::UnitDef*> defs;
		springai::Engine *engine;
		
		std::vector<springai::Resource*> resources;


	public:
		Utility( AIClasses* aiClasses );
		~Utility();
		springai::Map *map;
		
		double EuclideanDistance( SAIFloat3 pos1, SAIFloat3 pos2 );
		void Log(int logLevel, int logType, const char* msg, ...);
		void LogNN(int logLevel, int logType, const char* msg, ...);
		void ChatMsg(const char* msg, ...);
		void ChatMsg(std::string msg);
		springai::Unit* GiveUnit(int unitDefId);
		springai::Unit* GiveUnit(const char* unitDefName);
		float ReadBattleValue(int current_game, bool read_avarage=false);
		void WriteBattleValue(int current_game, float value);
		int GetCurrentGameNumber();
		void CalculateAverage();
		void DeleteUnits(std::vector<springai::Unit*> &units);
		springai::UnitDef* GetUnitDef(const char* unitDefName);
		springai::UnitDef* GetUnitDef(int unitDefId);
		springai::Resource* GetResource(const char* resourceName);
		int DrawCircle(SAIFloat3 pos, float radius );
		int DrawCircle(SAIFloat3 pos, float radius, int figureId);
		int DrawLine(SAIFloat3 start, SAIFloat3 end, bool arrow = false, float width = 20.0f, int figureId = 0 );
		void RemoveGraphics(int figureId);
		bool IsDebug();
		bool FileExists( const char* FileName );
		void LaterInitialization();
		int UnitsLeft(int teamId);
		void ResetGame();
		SAIFloat3 GetSafePosition();
		void SuicideNCUnits();
		void SuicideAllUnits();
		float GetDpsFromUnitDef(springai::UnitDef* def);
		void MoveUnits(std::vector<springai::Unit*> units, SAIFloat3 pos);
		void AttackArea(std::vector<springai::Unit*> units, SAIFloat3 pos);
	};
}
#endif
