
#include "../Classes/LuaAdapter.hpp"
#include "test_center.h"
#include <iostream>
#include <cstdlib>

static int __luaErrorHook(lua_State* L) {

	const string errorStr = lua_tostring(L, -1);

	cout << errorStr << endl;
	lua_Debug info;
	int level = 0;
	while (lua_getstack(L, level, &info)) {
		lua_getinfo(L, "nSl", &info);
		fprintf(stderr, "  [%d] %s:%d -- %s [%s]\n",
			level, info.short_src, info.currentline,
			(info.name ? info.name : "<unknown>"), info.what);
		++level;
	}

	return 1;
}

int main(int argc, char* argv[]){
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	lua_atpanic(L, __luaErrorHook);
	LuaAdapterEnvironment& la = LuaAdapter::LuaAdapterEnvironment::getInstance();
	la.init(L);

	LuaAdapter::luaadapter_test();
	system("pause");
}