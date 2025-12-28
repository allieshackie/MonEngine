#pragma once
extern "C" {
#include <lua.h>
}

class LuaContext
{
public:
	LuaContext();
	~LuaContext();

	// Copy constructor/assignment operator
	LuaContext(const LuaContext& other) = default;
	LuaContext& operator=(const LuaContext& other) = default;
	// Move constructor/assignment operator
	LuaContext(LuaContext&& other) noexcept = default;
	LuaContext& operator=(LuaContext&& rhs) noexcept = default;

	lua_State* GetState() const;
	void Execute(const char* scriptFile) const;
	void ExecuteWithInstance(const char* scriptFile, int tableRefIndex) const;

	void Initialize(int tableRefIndex) const;
	void Update(int tableRefIndex) const;

private:
	lua_State* mLuaState = nullptr;
};
