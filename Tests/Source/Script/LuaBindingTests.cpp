#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <glm/vec2.hpp>

#include "Script/LuaBindable.h"
#include "Script/LuaContext.h"
#include "Util/LuaUtil.h"

namespace
{
class TestBindable final : public LuaBindable
{
public:
    static constexpr char LuaName[] = "TestBindable";

    TestBindable()
        : LuaBindable(LuaName)
    {
    }

    int Add(int left, int right) const
    {
        return left + right;
    }

    void BindMethods(lua_State* state) override
    {
        LuaUtil::RegisterMethod<TestBindable, &TestBindable::Add>(state, "Add");
    }

    void BindInstanceGetter(lua_State* state) override
    {
        LuaUtil::RegisterInstanceGetter(state, "GetTestBindable", this);
    }
};

void RequireLuaSuccess(lua_State* state, int result)
{
    INFO("Lua error: " << (lua_tostring(state, -1) ? lua_tostring(state, -1) : "unknown error"));
    REQUIRE(result == LUA_OK);
}
}

TEST_CASE("LuaContext opens the standard Lua libraries", "[lua][context]")
{
    LuaContext context;
    lua_State* state = context.GetState();

    REQUIRE(state != nullptr);
    RequireLuaSuccess(state, luaL_dostring(state, "return math.floor(3.75)"));
    REQUIRE(lua_gettop(state) == 1);
    CHECK(lua_tointeger(state, -1) == 3);
    lua_pop(state, 1);
}

TEST_CASE("LuaBindable registers its metatable and instance getter", "[lua][binding]")
{
    LuaContext context;
    lua_State* state = context.GetState();

    lua_newtable(state);
    lua_setglobal(state, LuaUtil::mGameData.c_str());

    TestBindable subject;
    subject.Bind(state);

    REQUIRE(luaL_getmetatable(state, TestBindable::LuaName) == LUA_TTABLE);
    lua_getfield(state, -1, "Add");
    CHECK(lua_isfunction(state, -1));
    lua_pop(state, 1);

    lua_getfield(state, -1, "__index");
    CHECK(lua_rawequal(state, -1, -2));
    lua_pop(state, 2);

    lua_getglobal(state, LuaUtil::mGameData.c_str());
    REQUIRE(lua_istable(state, -1));
    lua_getfield(state, -1, "GetTestBindable");
    CHECK(lua_isfunction(state, -1));
    lua_pop(state, 2);

    CHECK(lua_gettop(state) == 0);

    RequireLuaSuccess(state, luaL_dostring(state, "return gGameData.GetTestBindable():Add(20, 22)"));
    REQUIRE(lua_gettop(state) == 1);
    CHECK(lua_tointeger(state, -1) == 42);
    lua_pop(state, 1);
}

TEST_CASE("Registered C++ methods can be called from Lua", "[lua][binding]")
{
    LuaContext context;
    lua_State* state = context.GetState();

    lua_newtable(state);
    lua_setglobal(state, LuaUtil::mGameData.c_str());

    auto subject = std::make_shared<TestBindable>();
    subject->Bind(state);
    LuaUtil::PushValue(state, subject);
    lua_setglobal(state, "subject");

    RequireLuaSuccess(state, luaL_dostring(state, "return subject:Add(20, 22)"));
    REQUIRE(lua_gettop(state) == 1);
    CHECK(lua_tointeger(state, -1) == 42);
    lua_pop(state, 1);
}
