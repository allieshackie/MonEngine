#include "Core/EngineContext.h"
#include "GUI/GUIMenuItems.h"
#include "GUI/GUISystem.h"
#include "GUI/GUIMenu.h"
#include "GUI/GUIMenuBar.h"

#include "LuaSystem.h"

//#define PROFILE_LUA

void LuaSystem::LoadScript(const char* scriptFile, const EngineContext& engine)
{
	auto context = std::make_unique<LuaContext>();
	const auto state = context->GetState();

	Mon::Lua::BindUIModule(state);
	Mon::Lua::BindEngineContext(state, engine);

	context->Execute(scriptFile);

	mContexts.push_back(std::move(context));
}

void Mon::Lua::BindEngineContext(lua_State* state, const EngineContext& engine)
{
	luaL_newmetatable(state, Engine::EngineTableName);

	// Add the getValue method to the metatable
	lua_pushcfunction(state, Mon::Engine::LoadLevel);
	lua_setfield(state, -2, "LoadLevel");

	// Add the getValue method to the metatable
	lua_pushcfunction(state, Mon::Engine::GetLevelNames);
	lua_setfield(state, -2, "GetLevelNames");

	lua_pushstring(state, "__index");
	lua_pushvalue(state, -2);
	lua_settable(state, -3);

	// Expose the C++ instance to Lua as a global variable
	auto userData = static_cast<const EngineContext**>(lua_newuserdata(state, sizeof(EngineContext*)));
	*userData = &engine;

	luaL_getmetatable(state, Engine::EngineTableName);
	lua_setmetatable(state, -2);

	lua_setglobal(state, "gEngineContext");
}

std::function<void()> Mon::Lua::GetFunctionFromLua(lua_State* L, int index)
{
	ProfileLuaState(L, true, "GetFunctionFromLua");
	luaL_checktype(L, index, LUA_TFUNCTION);

	// Push a copy of the function onto the stack
	lua_pushvalue(L, index);

	// Create a reference to the function
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);

	ProfileLuaState(L, false, "GetFunctionFromLua");

	// Define a lambda function that calls the Lua function
	return [L, ref]()
	{
		ProfileLuaState(L, true, "GetFunctionFromLua internal");
		// Retrieve the function from the registry
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);

		// Call the Lua function
		if (lua_pcall(L, 0, 0, 0) != LUA_OK)
		{
			const char* errorMessage = lua_tostring(L, -1);
			fprintf(stderr, "Error calling Lua function: %s\n", errorMessage);
			lua_pop(L, 1); // Pop the error message from the stack
		}

		ProfileLuaState(L, false, "GetFunctionFromLua internal");
	};
}

void Mon::Lua::ProfileLuaState(lua_State* L, bool before, const char* profileTag)
{
#ifdef PROFILE_LUA
	int stackSize = lua_gettop(L);
	printf("Tag: %s\n", profileTag);
	if (before)
	{
		printf("Stack size before: %d\n", stackSize);
	}
	else
	{
		printf("Stack size after: %d\n", stackSize);
	}
#endif
}


// ========== UI Module =================

void Mon::Lua::BindUIModule(lua_State* L)
{
	ProfileLuaState(L, true, "BindUIModule");
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

	// Create metatable for the MenuBar userdata
	luaL_newmetatable(L, UI::GUIComboTableName);
	luaL_setfuncs(L, Mon::UI::ComboTable, 0);

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

	ProfileLuaState(L, false, "BindUIModule");
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
		std::function<void()> clickCallback = Lua::GetFunctionFromLua(L, 7);

		auto button = std::make_unique<GUIButton>(label, glm::vec2{posX, posY}, glm::vec2{sizeX, sizeY},
		                                          std::move(clickCallback));
		(*menuPtr)->AddElement(std::move(button));
	}

	// Assuming the function does not return anything to Lua
	return 0;
}

int Mon::UI::Impl_AddCombo(lua_State* L)
{
	Lua::ProfileLuaState(L, true, "Impl_AddCombo");
	if (const auto menuPtr = static_cast<GUIMenu**>(luaL_checkudata(L, 1, MenuPopupTableName)))
	{
		const char* label = luaL_checkstring(L, 2);

		luaL_checktype(L, 3, LUA_TTABLE);

		// Get the array of strings from the Lua table
		std::vector<const char*> items;
		lua_pushnil(L); // Start with the first key in the Lua table 
		while (lua_next(L, 3) != 0)
		{
			// Assuming the Lua table only contains strings
			const char* item = lua_tostring(L, -1);
			items.push_back(item);

			// Pop the value, keep the key for the next iteration
			lua_pop(L, 1);
		}

		std::function<void()> changeCallback = Lua::GetFunctionFromLua(L, 4);

		auto combo = std::make_unique<GUICombo>(label, items, std::move(changeCallback));

		// Push a light userData (pointer to GUIMenu) to the Lua stack
		void* userData = lua_newuserdata(L, sizeof(GUICombo*));
		*(static_cast<GUICombo**>(userData)) = combo.get();

		(*menuPtr)->AddElement(std::move(combo));

		// Set meta table to associate user data obj, otherwise userdata remains loose data that
		// we can't perform any operations on 
		luaL_getmetatable(L, GUIComboTableName);
		lua_setmetatable(L, -2);

		Lua::ProfileLuaState(L, false, "Impl_AddCombo");

		return 1; // Return the userdata
	}

	// Assuming the function does not return anything to Lua
	return 0;
}

int Mon::UI::Impl_CreateMenuPopup(lua_State* L)
{
	Lua::ProfileLuaState(L, true, "Impl_CreateMenuPopup");

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

	Lua::ProfileLuaState(L, false, "Impl_CreateMenuPopup");

	return 1; // Return the userdata
}

int Mon::UI::Impl_CreateMenuBar(lua_State* L)
{
	Lua::ProfileLuaState(L, true, "Impl_CreateMenuBar");
	auto menu = std::make_unique<GUIMenuBar>();

	// Push a light userData (pointer to GUIMenu) to the Lua stack
	void* userData = lua_newuserdata(L, sizeof(GUIMenuBar*));
	*(static_cast<GUIMenuBar**>(userData)) = menu.get();

	// Set meta table to associate user data obj, otherwise userdata remains loose data that
	// we can't perform any operations on 
	luaL_getmetatable(L, MenuBarTableName);
	lua_setmetatable(L, -2);

	GUISystem::AddMenu(std::move(menu));

	Lua::ProfileLuaState(L, false, "Impl_CreateMenuBar");

	return 1; // Return the userdata
}

int Mon::UI::Impl_AddMenuBarItem(lua_State* L)
{
	Lua::ProfileLuaState(L, true, "Impl_AddMenuBarItem");
	if (const auto menuBarPtr = static_cast<GUIMenuBar**>(luaL_checkudata(L, 1, MenuBarTableName)))
	{
		const char* label = luaL_checkstring(L, 2);

		// Get the function pointer from the Lua function at index 3
		// TODO: Push value if we want to use the function callback later in the lua script
		// Ensures it doesn't pop off the stack
		//lua_pushvalue(L, 7);
		const std::function<void()> clickCallback = Lua::GetFunctionFromLua(L, 3);

		(*menuBarPtr)->AddMenuItem(label, clickCallback);
	}

	Lua::ProfileLuaState(L, false, "Impl_AddMenuBarItem");

	// Assuming the function does not return anything to Lua
	return 0;
}

int Mon::UI::Impl_GetCurrentItem(lua_State* L)
{
	Lua::ProfileLuaState(L, true, "Impl_GetCurrentItem");
	if (const auto comboPtr = static_cast<GUICombo**>(luaL_checkudata(L, 1, GUIComboTableName)))
	{
		lua_pushinteger(L, (*comboPtr)->GetCurrentItem());
		return 1;
	}

	Lua::ProfileLuaState(L, false, "Impl_GetCurrentItem");

	// Assuming the function does not return anything to Lua
	return 0;
}

int Mon::Engine::LoadLevel(lua_State* L)
{
	auto userdata = static_cast<EngineContext**>(luaL_checkudata(L, 1, EngineTableName));
	if (*userdata)
	{
		const char* label = luaL_checkstring(L, 2);
		(*userdata)->LoadLevel(label);
	}

	return 0;
}

int Mon::Engine::GetLevelNames(lua_State* L)
{
	auto userdata = static_cast<EngineContext**>(luaL_checkudata(L, 1, EngineTableName));
	if (*userdata)
	{
		const auto& levelNames = (*userdata)->GetLevelNames();
		// Create a Lua table to store the level names
		lua_newtable(L);

		// Iterate through the vector and push each string onto the Lua stack
		for (size_t i = 0; i < levelNames.size(); ++i)
		{
			// Push the string onto the Lua stack
			lua_pushstring(L, levelNames[i]);

			// Set the element at index i+1 in the Lua table
			lua_rawseti(L, -2, static_cast<int>(i + 1));
		}

		// Return the Lua table containing the level names
		return 1;
	}

	return 0;
}
