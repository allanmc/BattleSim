#include "Utility.h"
#include "global.h"
#include <stdio.h>
#include <sstream>

using namespace std;
using namespace springai;
using namespace brainSpace;

Utility::Utility( AIClasses* aiClasses )
{
	debug = true;
	ai = aiClasses;
	
	Cheats *c = ai->callback->GetCheats();
	c->SetEnabled(true);
	delete c;

	char filename[200];
	char path[200];
	DataDirs *dirs = DataDirs::GetInstance(ai->callback);
	
	if (LOG_LEVEL!=NONE) {
		const char *dir = dirs->GetWriteableDir();
		strcpy(path, dir);
		SNPRINTF( filename, 200, "Brain-log-team%d.txt", aiClasses->callback->GetTeamId() );
		strcat(path, filename);
		fp = FOPEN(path, "w");
	}
	Log(ALL, MISC, "Initialized Utility-class...");
	engine = ai->callback->GetEngine();
	map = ai->callback->GetMap();
	resources = ai->callback->GetResources();
	InitializeOptions();

	safePosition = (SAIFloat3){map->GetWidth()*8/2, 0.0, map->GetHeight()*8/2};

	if (ai->callback->GetTeamId()==0) {
		avarage_file = string(dirs->GetWriteableDir()) + "battles.dat";

		bfs = NULL;
		stringstream file_name;
		int current = 0;
		bool itsOkAlarm;
		do {
			itsOkAlarm = false;
			file_name.str("");
			if (bfs!=NULL) {bfs->close();}
			current++;
			file_name << dirs->GetWriteableDir();
			file_name << "battles";
			file_name <<  current;
			file_name << ".dat";
			Log(ALL, MISC, ("Constructor Checking " + file_name.str()).c_str());
			bfs = new fstream ( file_name.str().c_str() , ios::in) ;
			if ( bfs->is_open() )
			{
				long begin = bfs->tellg();
				bfs->seekg(0, ios::end);
				long length = (long)bfs->tellg() - begin - BATTLES_HEADER_SIZE;
				if ( length == TOTAL_NUMBER_OF_GAMES*4 )
				{
					itsOkAlarm = true;	
				}
				Log(ALL, MISC, "Current: %d, Length: %d, Games*4: %d", current, length, TOTAL_NUMBER_OF_GAMES*4);
			}
		} while (bfs->is_open() && itsOkAlarm );
		bfs->close();
		battles_file = file_name.str();

		bfs = new fstream( battles_file.c_str(), ios::out | ios::binary | ios::app);
	}
	
	delete dirs;
}


Utility::~Utility()
{
	if (LOG_LEVEL!=NONE) {
		fclose(fp);	
	}
	if (ai->callback->GetTeamId()==0) {
		bfs->close();
	}
	delete map;
	delete engine;
	for( unsigned int i = 0; i < defs.size(); i++)
	{
		delete defs[i];
	}

	for( unsigned int i = 0; i < resources.size() ; i++ )
	{
		delete resources[i];
	}
	Cheats *c = ai->callback->GetCheats();
	c->SetEnabled(false);
	delete c;
}

void Utility::CalculateAverage()
{
	if (ai->callback->GetTeamId()!=0) {
		return;
	}
	//Determine number of files to use for average
	DataDirs *dirs = DataDirs::GetInstance(ai->callback);
	bfs->close();
	stringstream file_name;
	int num_files = 0;
	do
	{
		file_name.str("");
		if (bfs!=NULL)
		{
			bfs->close();
		}
		num_files++;
		file_name << dirs->GetWriteableDir();
		file_name << "battles";
		file_name <<  num_files;
		file_name << ".dat";
		Log(ALL, MISC, ("CalculateAverage Checking " + file_name.str()).c_str());
		bfs = new fstream ( file_name.str().c_str() , ios::in) ;
	} while (bfs->is_open());

	--num_files;

	if (num_files<1)
	{
		return;
	}
	
	//Open all input files
	fstream* ifs[num_files];
	int current = 0;
	unsigned long num_bytes[num_files];
	unsigned long num_bytes_max = 0;
	while (current<num_files) {
		stringstream file_name;
		file_name << dirs->GetWriteableDir();
		file_name << "battles";
		file_name <<  current+1;
		file_name << ".dat";
		Log(ALL, MISC, ("Opening " + file_name.str()).c_str());
		ifs[current] = new fstream ( file_name.str().c_str() , ios::in | ios::binary) ;

		//Use file-size of first battle input file
		long begin = ifs[current]->tellg();
		ifs[current]->seekg(0, ios::end);
		num_bytes[current] = (long)ifs[current]->tellg() - begin - BATTLES_HEADER_SIZE;
		if ( num_bytes[current] > num_bytes_max ) {
			num_bytes_max = num_bytes[current];
		}
		current++;
	}
	
	Log(ALL, MISC, "AVERAGE BYTE SIZE = %i", num_bytes);

	//Calculate average, and write to output file
	fstream crap( avarage_file.c_str() , ios::out | ios::binary) ;
	crap.close();
	bfs->close();
	bfs = new fstream( avarage_file.c_str(), ios::in | ios::out | ios::binary );
	int num_values = num_bytes_max/sizeof(float);
	for (int i = 0 ; i < num_values; i++) {
		current = 0;
		float current_value;
		float current_average = 0.0;
		while (current<num_files)
		{
			Log(ALL, MISC, "Comparing: %i == %i", (i+1)*sizeof(float), num_bytes[current]);
			if ( (i+1)*sizeof(float) > num_bytes[current] )
			{
				current++;
				continue;
			}
			ifs[current]->seekg( BATTLES_HEADER_SIZE + i*sizeof(float) , ios::beg);
			ifs[current]->read( (char*)&current_value, sizeof(float) );
			Log(ALL, MISC, "Current value: %f", current_value);
			current_average=(current_average/(current+1))*current + current_value/(current+1);
			Log(ALL, MISC, "Current average: %f", current_average);
			current++;
		}
		WriteBattleValue(i, current_average);
	}
	Log(ALL, MISC, "Done with average");
	bfs->close();
	bfs = new fstream( battles_file.c_str(), ios::in | ios::out | ios::binary );
	Log(ALL, MISC, "Closing average");
	//Close input files
	current = 0;
	while (current<num_files) {
		ifs[current]->close();
		current++;
	}
	Log(ALL, MISC, "End average");
	delete dirs;
}

float Utility::ReadBattleValue(int current_game)
{
	if (ai->callback->GetTeamId()!=0) {
		return 0.0;
	}
	float value;
	bfs->seekg (BATTLES_HEADER_SIZE+current_game*sizeof(value), ios::beg);
	bfs->read( (char*)&value, sizeof(value) );
	return value;
}

void Utility::WriteBattleValue(int current_game, float value)
{
	
	ai->utility->Log( ALL, MISC, "Util::WriteBattleValue() current_game:%d", current_game );

	if (ai->callback->GetTeamId()!=0) {
		return;
	}
	bfs->seekp(BATTLES_HEADER_SIZE+current_game*sizeof(value), ios::beg);
	bfs->write( (char*)&value, sizeof(value) );
}

int Utility::GetCurrentGameNumber()
{
	DataDirs* dirs = DataDirs::GetInstance( ai->callback );
	if(!FileExists((string(dirs->GetWriteableDir())+"hest.dat").c_str()))
	{
		return -1;
	}
	ai->utility->Log( ALL, MISC, "Util::GetCurrentGameNumber()" );
	fstream* hest = new fstream( (string(dirs->GetWriteableDir())+"hest.dat").c_str() , ios::binary | ios::in);
	delete dirs;
	int count;
	hest->read((char*)&count, sizeof(count));
	//ai->utility->ChatMsg("Game start #%d", count);
	hest->close();
	delete hest;
	return count;
}

Unit* Utility::GiveUnit(const char* unitDefName)
{
	SGiveMeNewUnitCheatCommand giveUnitOrder;

	SAIFloat3 pos = ai->utility->GetSafePosition();

	if ( strcmp( unitDefName, "armthund" ) == 0 || strcmp( unitDefName, "armfig" ) == 0 || strcmp( unitDefName, "armkam" ) == 0 )
	{
		pos.x -= ( map->GetStartPos().x<map->GetWidth()*8/2 ? 700 : -700 );
	}
	else
		pos.x -= ( map->GetStartPos().x<map->GetWidth()*8/2 ? 300 : -300 );
	pos.x += ( rand() % 100 - 50);
	pos.z += ( rand() % 100 - 50);

	giveUnitOrder.pos = pos;
	giveUnitOrder.unitDefId = ai->utility->GetUnitDef(unitDefName)->GetUnitDefId();
	engine->HandleCommand(0,-1, COMMAND_CHEATS_GIVE_ME_NEW_UNIT, &giveUnitOrder);

	return Unit::GetInstance(ai->callback, giveUnitOrder.ret_newUnitId);
}

///@return the safe position whether a building blocks the exit of out base
SAIFloat3 Utility::GetSafePosition()
{
	return safePosition;
}

///@return if debug is enabled
bool Utility::IsDebug()
{
	return debug;
}

float Utility::GetDpsFromUnitDef(springai::UnitDef *def)
{
	vector<WeaponMount*> weaponMounts = def->GetWeaponMounts();
	float dmg = 0;
	for ( int i = 0 ; i < (int)weaponMounts.size() ; i++ )
	{
		WeaponDef* wDef = weaponMounts[i]->GetWeaponDef();
		dmg += (wDef->GetDamage()->GetTypes().at(def->GetArmorType()) / wDef->GetReload());
		delete wDef;
		delete weaponMounts[i];
	}
	return dmg;
}

void Utility::LaterInitialization()
{
	ai->utility->Log(ALL, MISC, "Calling GetUnitDefs");
	defs = ai->callback->GetUnitDefs();
	ai->utility->Log(ALL, MISC, "Done calling GetUnitDefs");
	Log(ALL, MISC, "LaterInitialization()");	
}

void Utility::DeleteUnits(vector<Unit*> &units) {
	for ( unsigned int i = 0 ; i < units.size() ; i++ ) {
		delete units[i];
	}
	units.clear();
}

int Utility::UnitsLeft(int teamId)
{
	int unitcount;
	vector<Unit*> units;
	if (teamId == ai->callback->GetTeamId()) {
		units = ai->callback->GetFriendlyUnits();
	} else {
		units = ai->callback->GetEnemyUnits();
	}
	unitcount = units.size();
	DeleteUnits(units);
	return unitcount;
}

///Load the options set for the AI
void Utility::InitializeOptions()
{
	OptionValues *opt = OptionValues::GetInstance(ai->callback);
	const char* optionStr = opt->GetValueByKey("debug");
	if (optionStr == NULL
		||
		strcmp(optionStr,"true")==0
		||
		strcmp(optionStr,"1")==0)
	{
		debug = true;
	}
	else debug = false;
	delete opt;
}

///Prints a line in the log file
///@param logLevel the minimum LOGLEVEL that this should be logged at
///@param logType the type of information being logged, used to filter logging
///@param msg the text to be printed in printf-syntax
void Utility::Log(int logLevel, int logType, const char* msg, ...)
{		
	if (!IsDebug() || LOG_LEVEL < logLevel)
	{
		return;
	}
	else if (((LOG_TYPE) & logType) == 0)
	{
		return;
	}
	
	if (fp)
	{
		fprintf(fp, "Frame %d: ", ai->frame);

		va_list list;
		va_start(list, msg);
		vfprintf(fp, msg, list);
		va_end(list);
		fputc('\n', fp);
		fflush(fp);
	}
}

///Prints a line in the log file - no new line
///@param logLevel the minimum LOGLEVEL that this should be logged at
///@param logType the type of information being logged, used to filter logging
///@param msg the text to be printed in printf-syntax
void Utility::LogNN(int logLevel, int logType, const char* msg, ...)
{		
	if (!IsDebug() || LOG_LEVEL < logLevel)
	{
		return;
	}
	else if (((LOG_TYPE) & logType) == 0)
	{
		return;
	}
	
	if (fp)
	{
		va_list list;
		va_start(list, msg);
		vfprintf(fp, msg, list);
		va_end(list);
		fflush(fp);
	}
}

///prints a line to the chat
void Utility::ChatMsg(const char* msg, ...)
{
	static char c[200];

	
	va_list list;
	va_start(list, msg);
	VSNPRINTF(c, 200, msg, list);
	va_end(list);
	SSendTextMessageCommand cmd;
	cmd.text = c;
	cmd.zone = 0;
	engine->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);

	LogNN(ALL, MISC, "Chat: ");
	Log(ALL, MISC, c);
}

///prints a line to the chat
void Utility::ChatMsg(std::string msg)
{
	if (strcmp(Info::GetInstance(ai->callback)->GetValueByKey("debug"),"true")!=0)
	{
		return;
	}

	SSendTextMessageCommand cmd;
	cmd.text = msg.c_str();
	cmd.zone = 0;
	engine->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);
}

///@return the UnitDef of with a given name, or NULL if the UnitDef does not exists
UnitDef* Utility::GetUnitDef(const char* unitDefName)
{
	if(ai->frame > 0)
	{
		for ( int i = 0 ; i < (int)defs.size() ; i++ )
		{
			if ( strcmp( defs[i]->GetName(), unitDefName ) == 0 )
			{
				return defs[i];
			}
		}
	}
	else
	{
		Log(ALL, MISC, "Could not GetUnitDef, because frame==0, CRITICAL ERROR!");
	}
	return NULL;
}

///@return the Resource with the given name, or NULL if the Resource does not exists
Resource* Utility::GetResource(const char* resourceName)
{
	Resource* retval = NULL;
	for ( int i = 0 ; i < (int)resources.size() ; i++ )
	{
		if ( strcmp( resources[i]->GetName(), resourceName ) == 0 )
		{
			retval = resources[i];			
		}
	}
	
	return retval;
}


///draws a circle on the map
int Utility::DrawCircle(SAIFloat3 pos, float radius)
{
	return DrawCircle(pos, radius, 0);
}

///draws a circle on the map given figureId
int Utility::DrawCircle(SAIFloat3 pos, float radius, int figureId)
{
	SCreateSplineFigureDrawerCommand circle;
	circle.lifeTime = 0;
	circle.figureGroupId = 0;
	circle.width = 20;
	circle.arrow = false;
	circle.figureGroupId = figureId;

	circle.pos1 = pos;
	circle.pos4 = pos;

	circle.pos1.z += radius;
	circle.pos4.x += radius;
	circle.pos2 = circle.pos1;
	circle.pos3 = circle.pos4;
	circle.pos2.x += MAGIC_CIRCLE_NUMBER*radius;
	circle.pos3.z += MAGIC_CIRCLE_NUMBER*radius;
	engine->HandleCommand(0, -1, COMMAND_DRAWER_FIGURE_CREATE_SPLINE, &circle);
	circle.figureGroupId = circle.ret_newFigureGroupId;

	circle.pos1 = circle.pos4;
	circle.pos4 = pos;
	circle.pos4.z -= radius;
	circle.pos2 = circle.pos1;
	circle.pos3 = circle.pos4;
	circle.pos2.z -= MAGIC_CIRCLE_NUMBER*radius;
	circle.pos3.x += MAGIC_CIRCLE_NUMBER*radius;
	engine->HandleCommand(0, -1, COMMAND_DRAWER_FIGURE_CREATE_SPLINE, &circle);

	circle.pos1 = circle.pos4;
	circle.pos4 = pos;
	circle.pos4.x -= radius;
	circle.pos2 = circle.pos1;
	circle.pos3 = circle.pos4;
	circle.pos2.x -= MAGIC_CIRCLE_NUMBER*radius;
	circle.pos3.z -= MAGIC_CIRCLE_NUMBER*radius;
	engine->HandleCommand(0, -1, COMMAND_DRAWER_FIGURE_CREATE_SPLINE, &circle);

	circle.pos1 = circle.pos4;
	circle.pos4 = pos;
	circle.pos4.z += radius;
	circle.pos2 = circle.pos1;
	circle.pos3 = circle.pos4;
	circle.pos2.z += MAGIC_CIRCLE_NUMBER*radius;
	circle.pos3.x -= MAGIC_CIRCLE_NUMBER*radius;
	engine->HandleCommand(0, -1, COMMAND_DRAWER_FIGURE_CREATE_SPLINE, &circle);

	AssignColorToGraphics( circle.figureGroupId );

	return circle.figureGroupId;
}

///draws a line on the map
int Utility::DrawLine(SAIFloat3 start, SAIFloat3 end, bool arrow, float width, int figureId )
{
	SCreateLineFigureDrawerCommand line;
	line.arrow = arrow;
	line.lifeTime = 0;
	line.width = width;
	line.pos1 = start;
	line.pos2 = end;
	line.figureGroupId = figureId;
	
	engine->HandleCommand(0,-1, COMMAND_DRAWER_FIGURE_CREATE_LINE, &line);

	AssignColorToGraphics( line.ret_newFigureGroupId );
	return line.ret_newFigureGroupId;
}

///removes all grephic elements with a given figureId
void Utility::RemoveGraphics(int figureId)
{
	SDeleteFigureDrawerCommand removeCmd;
	removeCmd.figureGroupId = figureId;
	engine->HandleCommand(0,-1, COMMAND_DRAWER_FIGURE_DELETE, &removeCmd);
}

///@return the direct distance bestween two points(2D)
double Utility::EuclideanDistance(SAIFloat3 pos1, SAIFloat3 pos2)
{
	return sqrt( pow( fabs( pos1.x - pos2.x ), 2 ) + pow( fabs( pos1.z - pos2.z ), 2  ) );
}

///colors all graphic objects with a given figureID
void Utility::AssignColorToGraphics( int figureGroupID )
{
	SSetColorFigureDrawerCommand color;
	color.figureGroupId = figureGroupID;
	color.alpha = 255;
	switch ( ai->callback->GetTeamId() )
	{
	case 0:
		color.color.x = TEAM_0_COLOR.x;
		color.color.y = TEAM_0_COLOR.y;
		color.color.z = TEAM_0_COLOR.z;
		break;
	case 1:
		color.color.x = TEAM_1_COLOR.x;
		color.color.y = TEAM_1_COLOR.y;
		color.color.z = TEAM_1_COLOR.z;
		break;
	case 2:
		color.color.x = TEAM_2_COLOR.x;
		color.color.y = TEAM_2_COLOR.y;
		color.color.z = TEAM_2_COLOR.z;
		break;
	case 3:
		color.color.x = TEAM_3_COLOR.x;
		color.color.y = TEAM_3_COLOR.y;
		color.color.z = TEAM_3_COLOR.z;
		break;
	case 4:
		color.color.x = TEAM_4_COLOR.x;
		color.color.y = TEAM_4_COLOR.y;
		color.color.z = TEAM_4_COLOR.z;
		break;
	case 5:
		color.color.x = TEAM_5_COLOR.x;
		color.color.y = TEAM_5_COLOR.y;
		color.color.z = TEAM_5_COLOR.z;
		break;
	case 6:
		color.color.x = TEAM_6_COLOR.x;
		color.color.y = TEAM_6_COLOR.y;
		color.color.z = TEAM_6_COLOR.z;
		break;
	case 7:
		color.color.x = TEAM_7_COLOR.x;
		color.color.y = TEAM_7_COLOR.y;
		color.color.z = TEAM_7_COLOR.z;
		break;
	}
	engine->HandleCommand(0,-1, COMMAND_DRAWER_FIGURE_SET_COLOR, &color);
}

bool Utility::FileExists( const char* FileName )
{
    FILE* fp = NULL;

    fp = fopen( FileName, "rb" );
    if( fp != NULL )
    {
        fclose( fp );
        return true;
    }

    return false;
}

void Utility::MoveUnits(vector<Unit*> units, SAIFloat3 pos)
{
	for( unsigned int i = 0 ; i < units.size() ; i++ )
	{
		SMoveUnitCommand com;
		com.toPos = pos;
		com.unitId = units[i]->GetUnitId();
		com.timeOut = 999999;
		com.options = NULL;
		engine->HandleCommand(0,-1, COMMAND_UNIT_MOVE, &com);
	}
}

void Utility::AttackArea(vector<Unit*> units, SAIFloat3 pos)
{
	//Log(ALL, BATTLESIM, "Attacking area with %d units", units.size());
	for( unsigned int i = 0 ; i < units.size() ; i++ )
	{
		SAttackAreaUnitCommand com;
		com.radius = 100;
		com.toAttackPos = pos;
		com.unitId = units[i]->GetUnitId();
		com.timeOut = 999999;
		com.options = 0;//UNIT_COMMAND_OPTION_SHIFT_KEY;
		engine->HandleCommand(0,-1, COMMAND_UNIT_ATTACK_AREA, &com);
	}
}

void Utility::SuicideNCUnits() {
	if (UnitsLeft(0) == 1 && UnitsLeft(1) == 1)
	{
		return;
	}
	ChatMsg("/luarules wipencunits");

}

void Utility::SuicideAllUnits()
{
	ChatMsg("/luarules wipeunits");
}
