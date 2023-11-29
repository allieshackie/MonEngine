#include "GUI/GUIButton.h"
#include "GUI/GUISystem.h"

#include "LuaSystem.h"

LuaSystem::LuaSystem()
{
	//BindUIModule();
}

void LuaSystem::LoadScript(const char* scriptFile)
{
	auto context = std::make_unique<LuaContext>();
	auto state = context->GetState();
	// Create metatable for the Menu userdata
	luaL_newmetatable(state, "MenuMetatable");

	// AddButton function the MetaTable
	lua_pushstring(state, "AddButton");
	lua_pushcfunction(state, Mon::UI::Impl_AddButton);
	lua_settable(state, -3);

	// __index is the entry that allows for calls like Table:Method()
	lua_pushstring(state, "__index");
	lua_pushvalue(state, -2);
	lua_settable(state, -3);

	lua_pushcfunction(state, Mon::UI::Impl_CreateMenu);
	lua_setglobal(state, "CreateMenu");

	context->Execute(scriptFile);
	mContexts.push_back(std::move(context));
}

void LuaSystem::BindUIModule()
{
	mUIModule = std::make_unique<LuaContext>();
	mUIModule->LoadWithoutExecute("MonUI.lua", true);
	const auto state = mUIModule->GetState();

	luaL_newmetatable(state, "UIMetaTable");

	luaL_newlibtable(state, Mon::UI::MenuTable);
	luaL_setfuncs(state, Mon::UI::MenuTable, 0);

	lua_setfield(state, -2, "__index");
	lua_pop(state, 1); /* pop metatable */


	// Register the object.func functions into the table that is at the top of the stack:
	//luaL_newlib(L, arraylib_f);

	lua_setglobal(state, "MonUI");
}

void LuaSystem::BindClassToLuaTable(lua_State* state, const char* tableName)
{
}

void LuaSystem::AddClassTableEntry()
{
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

int Mon::UI::Impl_Menu_GC(lua_State* L)
{
	// Retrieve the Menu object from the userdata
	GUIMenu* menu = *(static_cast<GUIMenu**>(luaL_checkudata(L, 1, "MenuMetatable")));

	// Perform any cleanup or resource deallocation here
	delete menu;
	return 0;
}

int Mon::UI::Impl_AddButton(lua_State* L)
{
	auto menuPtr = static_cast<GUIMenu**>(luaL_checkudata(L, 1, "MenuMetatable"));

	if (menuPtr)
	{
		const char* label = luaL_checkstring(L, 2);
		float posX = luaL_checknumber(L, 3);
		float posY = luaL_checknumber(L, 4);
		float sizeX = luaL_checknumber(L, 5);
		float sizeY = luaL_checknumber(L, 6);

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

int Mon::UI::Impl_CreateMenu(lua_State* L)
{
	const char* title = luaL_checkstring(L, 1);
	float posX = luaL_checknumber(L, 2);
	float posY = luaL_checknumber(L, 3);

	auto menu = std::make_unique<GUIMenu>(title, glm::vec2(posX, posY));

	// Push a light userData (pointer to GUIMenu) to the Lua stack
	void* userData = lua_newuserdata(L, sizeof(GUIMenu*));
	*(static_cast<GUIMenu**>(userData)) = menu.get();

	// Set meta table to associate user data obj, otherwise userdata remains loose data that
	// we can't perform any operations on 
	luaL_getmetatable(L, "MenuMetatable");
	lua_setmetatable(L, -2);

	GUISystem::AddMenu(std::move(menu));

	return 1; // Return the userdata
}
