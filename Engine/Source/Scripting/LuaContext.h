#pragma once
extern "C" {
#include <lua.h>
}

class LuaContext
{
public:
	LuaContext();
	~LuaContext();

	void Execute(const char* scriptFile, bool isModule = false) const;
	void LoadWithoutExecute(const char* scriptFile, bool isModule = false) const;

	lua_State* GetState() const;

private:
	lua_State* mLuaState = nullptr;
};
