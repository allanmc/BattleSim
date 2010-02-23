--
--  Info Definition Table format
--
--
--  These keywords must be lowercase for LuaParser to read them.
--
--  key:      user defined or one of the SKIRMISH_AI_PROPERTY_* defines in
--            SSkirmishAILibrary.h
--  value:    the value of the property
--  desc:     the description (could be used as a tooltip)
--
--
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------

local infos = {
	{
		key    = 'shortName',
		value  = 'BattleSim',
		desc   = 'BattleSim',
	},
	{
		key    = 'version',
		value  = '666', -- AI version - !This comment is used for parsing!
	},
	{
		key    = 'name',
		value  = 'BattleSim',
		desc   = 'BattleSim',
	},
	{
		key    = 'description',
		value  = 'This AI does not do much...',
		desc   = 'tooltip.',
	},
	{
		key    = 'url',
		value  = 'http://github.com/allanmc/Spring-brAIn',
		desc   = 'GitHub Project',
	},
	{
		key    = 'loadSupported',
		value  = 'no',
		desc   = 'whether this AI supports loading or not',
	},
	{
		key    = 'interfaceShortName',
		value  = 'C', -- AI Interface name - !This comment is used for parsing!
		desc   = 'the shortName of the AI interface this AI needs',
	},
	{
		key    = 'interfaceVersion',
		value  = '0.1', -- AI Interface version - !This comment is used for parsing!
		desc   = 'the minimum version of the AI interface this AI needs',
	},
	{
		key="debug",
		value="true",
		desc="Set BattleSim in debug mode? true/false",
	},
}

return infos
