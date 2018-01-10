
#include "adapter_lua.hpp"

size_t la_lua_table_len(lua_State* L, int idx) {
#if LUA_VERSION_NUM > 501
	lua_len(L, -1);
	auto len = lua_tointeger(L, -1);
	lua_pop(L, 1);
	return size_t(len);
#else
	return lua_objlen(L, -1);
#endif
}
