#include "LuaBindable.h"

void LuaBindable::Bind(lua_State* state)
{
	luaL_newmetatable(state, mId.c_str());

	BindMethods(state);

	lua_pushvalue(state, -1);
	lua_setfield(state, -2, "__index");

	lua_pop(state, 1);

	BindInstanceGetter(state);
}
