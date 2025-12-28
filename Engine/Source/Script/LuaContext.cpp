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

void LuaContext::Execute(const char* scriptFile) const
{
	std::string pathName = SCRIPTS_FOLDER;
	pathName.append(scriptFile);
	// [ ] loads chunk then cleans up
	if (luaL_dofile(mLuaState, pathName.c_str()) != LUA_OK)
	{
		fprintf(stderr, "Error: %s\n", lua_tostring(mLuaState, -1));
		// [ ] pops error, failure already pops chunk
		lua_pop(mLuaState, 1);
		lua_close(mLuaState);
	}

	// [ function ]
	lua_getglobal(mLuaState, "Initialize");

	if (lua_isfunction(mLuaState, -1))
	{
		// [ ] pops function
		if (lua_pcall(mLuaState, 0, 0, 0) != LUA_OK)
		{
			fprintf(stderr, "Error: %s\n", lua_tostring(mLuaState, -1));
			// [ ] pops error, failure already pops function
			lua_pop(mLuaState, 1);
		}
	}
	else
	{
		fprintf(stderr, "Error: Function not found\n");
		// [ ] cleanup function on failure
		lua_pop(mLuaState, 1);
	}
}

void LuaContext::ExecuteWithInstance(const char* scriptFile, int tableRefIndex) const
{
	std::string pathName = SCRIPTS_FOLDER;
	pathName.append(scriptFile);
	// [ chunk ]
	if (luaL_loadfile(mLuaState, pathName.c_str()) != LUA_OK)
	{
		fprintf(stderr, "Error: %s\n", lua_tostring(mLuaState, -1));
		// [ ] pops error, failure already pops chunk
		lua_pop(mLuaState, 1);
		return;
	}

	// [ chunk, table ]
	lua_rawgeti(mLuaState, LUA_REGISTRYINDEX, tableRefIndex);

	// [ chunk ], pops table
	lua_setupvalue(mLuaState, -2, 1);

	// [ ], pops chunk
	if (lua_pcall(mLuaState, 0, 0, 0) != LUA_OK)
	{
		fprintf(stderr, "Error: %s\n", lua_tostring(mLuaState, -1));
		// [ ] pops error, failure already pops chunk
		lua_pop(mLuaState, 1);
	}
}

void LuaContext::Initialize(int tableRefIndex) const
{
	// [ table ]
	lua_rawgeti(mLuaState, LUA_REGISTRYINDEX, tableRefIndex);
	// [ table, function ]
	lua_getfield(mLuaState, -1, "Initialize");

	if (!lua_isfunction(mLuaState, -1))
	{
		fprintf(stderr, "Error: Initialize is not a function\n");
		// [ ] pops error and table, function value would be nil in this case
		lua_pop(mLuaState, 2);
		return;
	}

	// [ table ] pops function
	lua_pcall(mLuaState, 0, 0, 0);

	// [ ] pops table
	lua_pop(mLuaState, 1);
}

void LuaContext::Update(int tableRefIndex) const
{
	// [ table ]
	lua_rawgeti(mLuaState, LUA_REGISTRYINDEX, tableRefIndex);
	// [ table, function ]
	lua_getfield(mLuaState, -1, "Update");

	if (!lua_isfunction(mLuaState, -1))
	{
		fprintf(stderr, "Error: Update is not a function\n");
		// [ ] pops error and table, function value would be nil in this case
		lua_pop(mLuaState, 2);
		return;
	}

	// [ table ] pops function
	lua_pcall(mLuaState, 0, 0, 0);

	// [ ] pops table
	lua_pop(mLuaState, 1);
}

lua_State* LuaContext::GetState() const
{
	return mLuaState;
}
