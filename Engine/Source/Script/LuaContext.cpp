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

	// [ chunk ]
	if (luaL_loadfile(mLuaState, pathName.c_str()) != LUA_OK)
	{
		fprintf(stderr, "Error: %s\n", lua_tostring(mLuaState, -1));
		lua_pop(mLuaState, 1);
		return;
	}

	// [ chunk, env ]
	lua_newtable(mLuaState);
	lua_newtable(mLuaState);
	lua_getglobal(mLuaState, "_G");
	lua_setfield(mLuaState, -2, "__index");
	lua_setmetatable(mLuaState, -2);

	// [ chunk, env, env ] — extra copy to keep around after setupvalue consumes one
	lua_pushvalue(mLuaState, -1);

	// [ chunk, env ] — setupvalue consumes the env duplicate, sets _ENV
	lua_setupvalue(mLuaState, -3, 1);

	// Stack is now [ chunk, env ]. Move env below chunk so chunk is callable on top.
	lua_insert(mLuaState, -2); // [ env, chunk ]

	if (lua_pcall(mLuaState, 0, 0, 0) != LUA_OK)
	{
		fprintf(stderr, "Error: %s\n", lua_tostring(mLuaState, -1));
		lua_pop(mLuaState, 2); // error + env
		return;
	}

	// [ env ] — pcall popped chunk, env remains on top
	lua_getfield(mLuaState, -1, "Initialize");

	if (lua_isfunction(mLuaState, -1))
	{
		if (lua_pcall(mLuaState, 0, 0, 0) != LUA_OK)
		{
			fprintf(stderr, "Error: %s\n", lua_tostring(mLuaState, -1));
			lua_pop(mLuaState, 1);
		}
	}
	else
	{
		fprintf(stderr, "Error: Function not found\n");
		lua_pop(mLuaState, 1);
	}

	lua_pop(mLuaState, 1); // pop env
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

void LuaContext::CallMethod(int tableRefIndex, const char* methodName)
{
	// [ table ]
	lua_rawgeti(mLuaState, LUA_REGISTRYINDEX, tableRefIndex);
	// [ table, function ]
	lua_getfield(mLuaState, -1, methodName);

	if (!lua_isfunction(mLuaState, -1))
	{
		std::string error = "Error: ";
		error.append(methodName);
		error.append("is not a function");
		fprintf(stderr, error.c_str());
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
