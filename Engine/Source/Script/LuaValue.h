#pragma once
#include <lua.h>

class LuaValue
{
public:
	template <typename T>
	void Bind(lua_State* state, T& value)
	{
	}

	template <typename T>
	T& Get(lua_State* state)
	{
	}
};
