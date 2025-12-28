#pragma once
extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

class LuaBindable
{
public:
	LuaBindable(std::string id) : mId(std::move(id))
	{
	}

	virtual ~LuaBindable() = default;

	void Bind(lua_State* state);
	virtual void BindMethods(lua_State* state) = 0;
	virtual void BindInstanceGetter(lua_State* state) = 0;

private:
	std::string mId;
};
