#include "RegisterImplementation.hpp"

void LuaAdapter::nameCheck(lua_State* L, const string& name){
#ifdef _DEBUG
	lua_pushstring(L, name.c_str());
	lua_rawget(L, -2);
	assert(lua_isnil(L, -1));
	lua_pop(L, 1);
#endif // _DEBUG
}