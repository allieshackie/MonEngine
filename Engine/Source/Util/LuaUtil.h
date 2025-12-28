#pragma once
#include "Util/Logger.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

namespace LuaUtil
{
	inline std::string mGameData = "gGameData";

	template <typename T>
	void PushValue(lua_State* state, T value)
	{
		using baseType = std::remove_cv_t<std::remove_reference_t<T>>;
		if constexpr (std::is_floating_point_v<T>)
		{
			lua_pushnumber(state, value);
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			lua_pushboolean(state, value);
		}
		else if constexpr (std::is_integral_v<T>)
		{
			lua_pushinteger(state, value);
		}
		else if constexpr (std::is_same_v<baseType, std::string>)
		{
			lua_pushlstring(state, value.c_str(), value.size());
		}
		else
		{
			MON_WARN("No push for value type");
		}
	}

	template <typename T>
	T GetValue(lua_State* state, int index)
	{
		if constexpr (std::is_floating_point_v<T>)
		{
			return static_cast<T>(luaL_checknumber(state, index));
		}
		else if constexpr (std::is_integral_v<T>)
		{
			return static_cast<T>(luaL_checkinteger(state, index));
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			return lua_toboolean(state, index) != 0;
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			const char* s = luaL_checkstring(state, index);
			return std::string(s);
		}
		else
		{
			static_assert(!sizeof(T), "No control case for GetValue");
			return T{};
		}
	}

	template <typename T>
	struct LuaFuncTraits;

	// non-const member function
	template <class C, class Ret, class... Args>
	struct LuaFuncTraits<Ret(C::*)(Args ...)>
	{
		using Class = C;
		using Return = Ret;
		using ArgsTuple = std::tuple<Args...>;
	};

	// const method version
	template <class C, class Ret, class... Args>
	struct LuaFuncTraits<Ret(C::*)(Args ...) const>
	{
		using Class = C;
		using Return = Ret;
		using ArgsTuple = std::tuple<Args...>;
	};

	template <auto Method, typename Traits, std::size_t... I>
	int PushMethodImpl(lua_State* state, typename Traits::Class* self, std::index_sequence<I...>)
	{
		using Ret = typename Traits::Return;
		using ArgsTuple = typename Traits::ArgsTuple;

		if constexpr (std::is_void_v<Ret>)
		{
			(self->*Method)(
				LuaUtil::GetValue<std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<I, ArgsTuple>>>>(
					state, 2 + I)...);
			return 0;
		}
		else
		{
			Ret result = (self->*Method)(
				LuaUtil::GetValue<std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<I, ArgsTuple>>>>(
					state, 2 + I)...);
			LuaUtil::PushValue<Ret>(state, result);
			return 1;
		}
	}

	template <auto Method, typename Ret, typename Tuple>
	int PushMethod(lua_State* state, typename LuaFuncTraits<decltype(Method)>::Class* self)
	{
		using Traits = LuaFuncTraits<decltype(Method)>;
		return PushMethodImpl<Method, Traits>(state, self,
		                                      std::make_index_sequence<std::tuple_size_v<typename Traits::ArgsTuple>>
		                                      {});
	}

	template <auto Method>
	int LuaFunc(lua_State* state)
	{
		using Traits = LuaFuncTraits<decltype(Method)>;
		using C = typename Traits::Class;
		using Ret = typename Traits::Return;
		using ArgsTuple = typename Traits::ArgsTuple;

		C** selfPtr = static_cast<C**>(luaL_checkudata(state, 1, C::LuaName));
		C* self = *selfPtr;

		return LuaUtil::PushMethod<Method, Ret, ArgsTuple>(state, self);
	}

	template <class T, auto Method>
	void RegisterMethod(lua_State* state, const char* name)
	{
		lua_pushcfunction(state, &LuaFunc<Method>);
		lua_setfield(state, -2, name);
	}

	template <class T>
	int PushInstance(lua_State* state, T* instance)
	{
		if (instance)
		{
			T** udata = static_cast<T**>(lua_newuserdata(state, sizeof(T*)));
			*udata = instance;

			luaL_getmetatable(state, T::LuaName);
			lua_setmetatable(state, -2);
			return 1;
		}
		lua_pushnil(state);
		return 1;
	}

	template <class T>
	void RegisterInstanceGetter(lua_State* state, const char* name, T* instance)
	{
		lua_getglobal(state, mGameData.c_str());

		T** data = static_cast<T**>(lua_newuserdata(state, sizeof(T*)));
		*data = instance;

		lua_pushcclosure(state, [](lua_State* state) -> int
		{
			T* inst = *static_cast<T**>(lua_touserdata(state, lua_upvalueindex(1)));
			return PushInstance<T>(state, inst);
		}, 1);

		lua_setfield(state, -2, name);
		lua_pop(state, 1);
	}

	template <class T>
	int CreateLuaTable(lua_State* state, T* instance)
	{
		lua_newtable(state);

		int envIndex = lua_gettop(state);
		PushInstance<T>(state, instance);
		lua_setfield(state, envIndex, "entity");

		lua_newtable(state);
		lua_pushglobaltable(state);
		lua_setfield(state, -2, "__index");
		lua_setmetatable(state, envIndex);

		return luaL_ref(state, LUA_REGISTRYINDEX);
	}
};
