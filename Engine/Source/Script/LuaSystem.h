#pragma once
extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include "LuaContext.h"

namespace LUA_UTIL
{
	template <typename T, typename... Args>
	void BindFunction(lua_State* state, Args&&... args)
	{
	}

	template <typename T>
	void BindValue(lua_State* state, T& value)
	{
	}

	template <typename T>
	void BindInstance(lua_State* state)
	{
	}
}

class LuaSystem
{
public:
	LuaSystem();

	void LoadScript(const char* scriptFile);

	void QueueClose();
	void CloseAllScripts();

	bool QueueCloseScripts() const;

private:
	std::unique_ptr<LuaContext> mLuaContext;
	bool mQueueClose = false;
};
