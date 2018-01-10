#include "../Classes/LuaAdapter.hpp"
#include "test_center.h"
#include "construction_destruction.h"
#include "simple_test.h"
#include "extends_test.h"

namespace LuaAdapter {

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

	void luaadapter_test() {
		lua_State* L = luaL_newstate();
		luaL_openlibs(L);
		lua_atpanic(L, __luaErrorHook);
		LuaAdapterEnvironment& la = LuaAdapter::LuaAdapterEnvironment::getInstance();
		la.init(L);

		simple_test::simple_test();
	
		extends_test::extends_test();

		cons_des_test::test();
		
	}

}