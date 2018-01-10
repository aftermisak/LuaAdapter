#include "simple_test.h"
#include "../Classes/LuaAdapter.hpp"

using namespace LuaAdapter;
using namespace LuaAdapter::simple_test;

namespace LuaAdapter {
	namespace simple_test{
		unsigned ClassA::staticVariable1 = 0u ;
		const int ClassA::staticVariable2 = 1;
	}
}
static ClassA* createA() {
	return new ClassA;
}

void LuaAdapter::simple_test::simple_test() {
	auto& lae = LuaAdapter::LuaAdapterEnvironment::getInstance();
	lae.begin();
	lae.registerNamespace("simple_test");
	auto ns = lae.getNamespace("simple_test");
	ns->begin();
	{
		ns->registerClass("simple_test.ClassA", typeid(ClassA));
		auto cls = ns->getClass("simple_test.ClassA");
		cls->begin();
		cls->registerFunction(&createA, "createA");
		cls->registerVariable(&ClassA::memberVariable1, "memberVariable1");
		cls->registerVariable(&ClassA::memberVariable2, "memberVariable2");
		cls->registerVariable(&ClassA::staticVariable1, "staticVariable1");
		cls->registerVariable(&ClassA::staticVariable2, "staticVariable2");
		cls->registerFunction(&ClassA::memberFunction1, "memberFunction1");
		cls->registerFunction(&ClassA::memberFunction2, "memberFunction2");
		cls->registerFunction(&ClassA::memberFunction3, "memberFunction3");
		cls->registerFunction(&ClassA::staticFunction1, "staticFunction1");
		cls->registerFunction(&ClassA::staticFunction2, "staticFunction2");
		cls->end();
	}
	ns->end();
	lae.end();

	auto L = lae.getLua_State();
	int const loadStatus = luaL_loadfile(L, "LuaAdapter/testLuaSrc/test_simple_test.lua");
	if (LUA_OK != loadStatus) {
		cout << lua_tostring(L, -1) << endl;
		assert(false);
	}
	lua_call(L, 0, 0);

	//end simple_test
}
