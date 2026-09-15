#pragma once
extern "C" {
#include <lua.h>
}

class LuaContext
{
public:
	LuaContext();
	~LuaContext();

	LuaContext(const LuaContext& other) = delete;
	LuaContext& operator=(const LuaContext& other) = delete;
	LuaContext(LuaContext&& other) noexcept = delete;
	LuaContext& operator=(LuaContext&& rhs) noexcept = delete;

	lua_State* GetState() const;
	void Execute(const char* scriptFile) const;
	void ExecuteWithInstance(const char* scriptFile, int tableRefIndex) const;

	void Initialize(int tableRefIndex) const;
	void Update(int tableRefIndex) const;

	void CallMethod(int tableRefIndex, const char* methodName);

private:
	lua_State* mLuaState = nullptr;
};
