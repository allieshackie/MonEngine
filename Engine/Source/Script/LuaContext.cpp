extern "C" {
#include <lualib.h>
#include <lauxlib.h>
}

#include "LuaContext.h"

LuaContext::LuaContext()
{
	mLuaState = luaL_newstate();
	luaL_openlibs(mLuaState); // Open standard Lua libraries
}

LuaContext::~LuaContext()
{
	lua_close(mLuaState);
}

void LuaContext::Execute(const char* scriptFile, bool isModule) const
{
	std::string pathName = SCRIPTS_FOLDER;
	pathName.append(scriptFile);
	// Load and execute Lua script
	if (luaL_dofile(mLuaState, pathName.c_str()) != 0)
	{
		// Handle errors (e.g., print the error message)
		fprintf(stderr, "Error: %s\n", lua_tostring(mLuaState, -1));
		lua_pop(mLuaState, 1); // Pop the error message from the stack
		lua_close(mLuaState);
	}

	// Call a specific Lua function
	lua_getglobal(mLuaState, "Initialize");

	// Check if the Lua function is callable
	if (lua_isfunction(mLuaState, -1))
	{
		// Call the Lua function with arguments and expect one result
		if (lua_pcall(mLuaState, 0, 0, 0) != 0)
		{
			// Handle errors (e.g., print the error message)
			fprintf(stderr, "Error: %s\n", lua_tostring(mLuaState, -1));
			lua_pop(mLuaState, 1); // Pop the error message from the stack
		}
	}
	else
	{
		fprintf(stderr, "Error: Function not found\n");
	}
}

void LuaContext::LoadWithoutExecute(const char* scriptFile, bool isModule) const
{
	std::string pathName = SCRIPTS_FOLDER;
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
