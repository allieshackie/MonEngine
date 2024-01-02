#include "GUI/GUIButton.h"
#include "GUI/GUISystem.h"
#include "GUI/GUIMenu.h"
#include "GUI/GUIMenuBar.h"

#include "LuaSystem.h"

void LuaSystem::LoadScript(const char* scriptFile)
{
	auto context = std::make_unique<LuaContext>();
	const auto state = context->GetState();

	Mon::LuaHelper::BindUIModule(state);

	context->Execute(scriptFile);

	// TODO: Is this list necessary? 
	mContexts.push_back(std::move(context));
}

std::function<void()> Mon::LuaHelper::GetFunctionFromLua(lua_State* L, int index)
{
	luaL_checktype(L, index, LUA_TFUNCTION);

	// Push a copy of the function onto the stack
	lua_pushvalue(L, index);

	// Create a reference to the function
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);

	// Define a lambda function that calls the Lua function
	return [L, ref]()
	{
		// Retrieve the function from the registry
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);

		// Call the Lua function
		if (lua_pcall(L, 0, 0, 0) != LUA_OK)
		{
			const char* errorMessage = lua_tostring(L, -1);
			fprintf(stderr, "Error calling Lua function: %s\n", errorMessage);
			lua_pop(L, 1); // Pop the error message from the stack
		}
	};
}


// ========== UI Module =================

void Mon::LuaHelper::BindUIModule(lua_State* L)
{
	// Create metatable for the MenuPopup userdata
	luaL_newmetatable(L, UI::MenuPopupTableName);
	luaL_setfuncs(L, Mon::UI::MenuPopupTable, 0);

	// __index is the entry that allows for calls like Table:Method()
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	// Create metatable for the MenuBar userdata
	luaL_newmetatable(L, UI::MenuBarTableName);
	luaL_setfuncs(L, Mon::UI::MenuBarTable, 0);

	// __index is the entry that allows for calls like Table:Method()
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	// Create a table and set the "CreateMenuPopup" method in it
	luaL_newlib(L, Mon::UI::MenuModuleTable);

	// Create metatable
	lua_newtable(L);

	// Set __index in the metatable to point to itself (to allow recursive indexing)
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	// Set the metatable for the table
	lua_setmetatable(L, -2);

	// Set the table in the global environment
	lua_setglobal(L, "MonUI");
}

int Mon::UI::Impl_AddButton(lua_State* L)
{
	if (const auto menuPtr = static_cast<GUIMenu**>(luaL_checkudata(L, 1, MenuPopupTableName)))
	{
		const char* label = luaL_checkstring(L, 2);
		const float posX = luaL_checknumber(L, 3);
		const float posY = luaL_checknumber(L, 4);
		const float sizeX = luaL_checknumber(L, 5);
		const float sizeY = luaL_checknumber(L, 6);

		// Get the function pointer from the Lua function at index 7
		// TODO: Push value if we want to use the function callback later in the lua script
		// Ensures it doesn't pop off the stack
		//lua_pushvalue(L, 7);
		const std::function<void()> clickCallback = LuaHelper::GetFunctionFromLua(L, 7);

		(*menuPtr)->AddButton(label, posX, posY, sizeX, sizeY, clickCallback);
	}

	// Assuming the function does not return anything to Lua
	return 0;
}

int Mon::UI::Impl_CreateMenuPopup(lua_State* L)
{
	const char* title = luaL_checkstring(L, 1);
	const float posX = luaL_checknumber(L, 2);
	const float posY = luaL_checknumber(L, 3);
	const float sizeX = luaL_checknumber(L, 4);
	const float sizeY = luaL_checknumber(L, 5);

	auto menu = std::make_unique<GUIMenu>(title, glm::vec2(posX, posY), glm::vec2(sizeX, sizeY));

	// Push a light userData (pointer to GUIMenu) to the Lua stack
	void* userData = lua_newuserdata(L, sizeof(GUIMenu*));
	*(static_cast<GUIMenu**>(userData)) = menu.get();

	// Set meta table to associate user data obj, otherwise userdata remains loose data that
	// we can't perform any operations on 
	luaL_getmetatable(L, MenuPopupTableName);
	lua_setmetatable(L, -2);

	GUISystem::AddMenu(std::move(menu));

	return 1; // Return the userdata
}

int Mon::UI::Impl_CreateMenuBar(lua_State* L)
{
	auto menu = std::make_unique<GUIMenuBar>();

	// Push a light userData (pointer to GUIMenu) to the Lua stack
	void* userData = lua_newuserdata(L, sizeof(GUIMenuBar*));
	*(static_cast<GUIMenuBar**>(userData)) = menu.get();

	// Set meta table to associate user data obj, otherwise userdata remains loose data that
	// we can't perform any operations on 
	luaL_getmetatable(L, MenuBarTableName);
	lua_setmetatable(L, -2);

	GUISystem::AddMenu(std::move(menu));

	return 1; // Return the userdata
}

int Mon::UI::Impl_AddMenuBarItem(lua_State* L)
{
	if (const auto menuBarPtr = static_cast<GUIMenuBar**>(luaL_checkudata(L, 1, MenuBarTableName)))
	{
		const char* label = luaL_checkstring(L, 2);

		// Get the function pointer from the Lua function at index 3
		// TODO: Push value if we want to use the function callback later in the lua script
		// Ensures it doesn't pop off the stack
		//lua_pushvalue(L, 7);
		const std::function<void()> clickCallback = LuaHelper::GetFunctionFromLua(L, 3);

		(*menuBarPtr)->AddMenuItem(label, clickCallback);
	}

	// Assuming the function does not return anything to Lua
	return 0;
}
