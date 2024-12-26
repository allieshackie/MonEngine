#pragma once
extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include "LuaContext.h"

class EngineContext;

namespace Mon
{
	namespace Lua
	{
		void BindUIModule(lua_State* L);
		void BindEngineContext(lua_State* state, const EngineContext& engine);

		std::function<void()> GetFunctionFromLua(lua_State* L, int index);
		void ProfileLuaState(lua_State* L, bool before, const char* profileTag);
	}

	namespace UI
	{
		int Impl_CreateMenuPopup(lua_State* L);
		int Impl_CreateMenuBar(lua_State* L);

		inline const char* MenuPopupTableName = "MenuPopupTable";
		int Impl_AddButton(lua_State* L);
		int Impl_AddCombo(lua_State* L);
		int Impl_SetNoMoveFlag(lua_State* L);
		int Impl_SetNoResizeFlag(lua_State* L);

		inline const char* MenuBarTableName = "MenuBarTable";
		int Impl_AddMenuBarItem(lua_State* L);

		inline const char* GUIComboTableName = "GUIComboTable";
		int Impl_GetCurrentItem(lua_State* L);

		constexpr luaL_Reg MenuModuleTable[] = {
			{"CreateMenuPopup", Impl_CreateMenuPopup},
			{"CreateMenuBar", Impl_CreateMenuBar},
			{nullptr, nullptr} // Terminating element
		};

		constexpr luaL_Reg MenuPopupTable[] = {
			{"AddButton", Impl_AddButton},
			{"AddCombo", Impl_AddCombo},
			{"SetNoMoveFlag", Impl_SetNoMoveFlag},
			{"SetNoResizeFlag", Impl_SetNoResizeFlag},
			{nullptr, nullptr} // Sentinel to indicate the end of the array
		};

		constexpr luaL_Reg MenuBarTable[] = {
			{"AddMenuBarItem", Impl_AddMenuBarItem},
			{nullptr, nullptr} // Sentinel to indicate the end of the array
		};

		constexpr luaL_Reg ComboTable[] = {
			{"GetCurrentItem", Impl_GetCurrentItem},
			{nullptr, nullptr} // Sentinel to indicate the end of the array
		};
	}

	namespace Engine
	{
		inline const char* EngineTableName = "EngineMetaTable";
		int LoadScene(lua_State* L);
		int GetSceneNames(lua_State* L);
		int OpenEditorMenu(lua_State* L);
	}
}

class LuaSystem
{
public:
	void LoadScript(const char* scriptFile, const EngineContext& engine);

	void QueueClose();
	void CloseAllScripts();

	bool QueueCloseScripts() const;

private:
	std::vector<std::unique_ptr<LuaContext>> mContexts;
	bool mQueueClose = false;
};
