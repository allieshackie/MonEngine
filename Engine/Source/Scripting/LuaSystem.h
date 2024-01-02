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
		void BindUIModule(lua_State* L);
		std::function<void()> GetFunctionFromLua(lua_State* L, int index);
	}

	namespace UI
	{
		int Impl_CreateMenuPopup(lua_State* L);
		int Impl_CreateMenuBar(lua_State* L);

		inline const char* MenuPopupTableName = "MenuPopupTable";
		int Impl_AddButton(lua_State* L);

		inline const char* MenuBarTableName = "MenuBarTable";
		int Impl_AddMenuBarItem(lua_State* L);

		constexpr luaL_Reg MenuModuleTable[] = {
			{"CreateMenuPopup", Impl_CreateMenuPopup},
			{"CreateMenuBar", Impl_CreateMenuBar},
			// TODO: Add more functions here as needed
			{nullptr, nullptr} // Terminating element
		};

		constexpr luaL_Reg MenuPopupTable[] = {
			{"AddButton", Impl_AddButton},
			{nullptr, nullptr} // Sentinel to indicate the end of the array
		};

		constexpr luaL_Reg MenuBarTable[] = {
			{"AddMenuBarItem", Impl_AddMenuBarItem},
			{nullptr, nullptr} // Sentinel to indicate the end of the array
		};
	}
}

class LuaSystem
{
public:
	void LoadScript(const char* scriptFile);

private:
	std::vector<std::unique_ptr<LuaContext>> mContexts;
};
