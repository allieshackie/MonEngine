extern "C" {
#include <lualib.h>
#include <lauxlib.h>
}

#include "LuaContext.h"

LuaContext::LuaContext()
{
	mLuaState = luaL_newstate();
	luaL_openlibs(mLuaState); // Open standard Lua libraries


	/*
	 *
	if (lua_pcall(mLuaState, 0, 0, 0) != LUA_OK)
	{
		MON_WARN("Filed to compile lua script");
		return;
	}

	// TODO: This is assuming that the script will always have a main method
	// Call the Lua function "Main"
	lua_getglobal(mLuaState, "Main");
	if (lua_isfunction(mLuaState, -1))
	{
		lua_pcall(mLuaState, 0, 0, 0);
	}
	 */
}

LuaContext::~LuaContext()
{
	lua_close(mLuaState);
}

void LuaContext::Execute(const char* scriptFile, bool isModule) const
{
	std::string pathName = isModule ? LUA_MODULES_FOLDER : SCRIPTS_FOLDER;
	pathName.append(scriptFile);
	// Load the file, but don't run yet
	if (luaL_dofile(mLuaState, pathName.c_str()) != LUA_OK)
	{
		// Handle script loading or execution errors
		const char* errorMessage = lua_tostring(mLuaState, -1);
		fprintf(stderr, "Error executing Lua script: %s\n", errorMessage);
	}
}

void LuaContext::LoadWithoutExecute(const char* scriptFile, bool isModule) const
{
	std::string pathName = isModule ? LUA_MODULES_FOLDER : SCRIPTS_FOLDER;
	pathName.append(scriptFile);
	// Load the file, but don't run yet
	if (luaL_loadfile(mLuaState, pathName.c_str()) != LUA_OK)
	{
		// Handle script loading or execution errors
		const char* errorMessage = lua_tostring(mLuaState, -1);
		fprintf(stderr, "Error loading Lua script: %s\n", errorMessage);
	}
}

lua_State* LuaContext::GetState() const
{
	return mLuaState;
}
