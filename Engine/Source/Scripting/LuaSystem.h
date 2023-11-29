#pragma once
extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include "LuaContext.h"

namespace Mon
{
	namespace LuaHelper
	{
		static std::function<void()> GetFunctionFromLua(lua_State* L, int index);
	}

	namespace UI
	{
		int Impl_CreateMenu(lua_State* L);
		int Impl_AddButton(lua_State* L);
		int Impl_Menu_GC(lua_State* L);

		constexpr luaL_Reg MenuTable[] = {
			{"CreateMenu", Impl_CreateMenu},
			// TODO: Add more functions here as needed
			{nullptr, nullptr} // Terminating element
		};
	}
}

class LuaSystem
{
public:
	LuaSystem();

	void LoadScript(const char* scriptFile);
	void BindUIModule();


	void BindClassToLuaTable(lua_State* state, const char* tableName);
	// Any entries need to be added as soon as the table is added
	void AddClassTableEntry();

private:
	std::vector<std::unique_ptr<LuaContext>> mContexts;

	std::unique_ptr<LuaContext> mUIModule;
};
