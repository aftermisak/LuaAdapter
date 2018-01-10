#include "extends_test.h"
#include "../Classes/LuaAdapter.hpp"
#include <typeindex>
using namespace LuaAdapter;
using namespace LuaAdapter::extends_test;

namespace LuaAdapter {

namespace extends_test {

	const char* ClassSupperA::post_code = "610000";

	template<typename T>
	struct SimpleCreate {
		static T* func() {
			return new T;
		}
	};

	template<>
	struct SimpleCreate<ClassSubA> {
		typedef ClassSubA TargetType;
		static TargetType* func() {
			auto instance = new TargetType;
			return instance;
		}
	};

	template<typename T>
	struct TypeIndexWrap{
		
	};

	void processClassSupperA(ClassSupperA* p) {
		std::cout << "processClassSupperA" << p->getMyDog() << std::endl;
	}
	void processClassSupperB(ClassSupperB* p) {
		std::cout << "processClassSupperB" << p->getMyCat() << std::endl;
	}
	void processClassSubA(ClassSubA* p) {
		std::cout << "processClassSubA" << p->getMyDog() <<
			p->getMyCat() << std::endl;
	}
	void processClassSubB(ClassSubB* p) {
		std::cout << "processClassSubB" << p->getMyGirlfriend() <<
			p->getMyCat() << std::endl;
	}

	
}//end ns extends_test
}


void LuaAdapter::extends_test::extends_test() {
	
	auto& lae = LuaAdapter::LuaAdapterEnvironment::getInstance();
	lae.begin();
	lae.registerNamespace("extends_test");
	auto ns = lae.getNamespace("extends_test");
	ns->begin();
	ns->registerFunction(processClassSupperA, "processClassSupperA");
	ns->registerFunction(processClassSupperB, "processClassSupperB");
	ns->registerFunction(processClassSubA, "processClassSubA");
	ns->registerFunction(processClassSubB, "processClassSubB");
	{
		ns->registerClass("ext_ClassSupperA", typeid(ClassSupperA));
		auto cls = ns->getClass("ext_ClassSupperA");
		cls->begin();
		cls->registerVariable(&ClassSupperA::post_code, "post_code");
		cls->registerFunction(&ClassSupperA::getMyDog, "getMyDog");
		cls->registerFunction(&ClassSupperA::setMyDog, "setMyDog");
		cls->registerFunction(SimpleCreate<ClassSupperA>::func, "simpleCreate");
		cls->end();

		ns->registerClass("ext_ClassSupperB", typeid(ClassSupperB));
		cls = ns->getClass("ext_ClassSupperB");
		cls->begin();
		cls->registerFunction(&ClassSupperB::getMyCat, "getMyCat");
		cls->registerFunction(&ClassSupperB::yield, "yield");
		cls->registerFunction(SimpleCreate<ClassSupperB>::func, "simpleCreate");
		cls->end();


		ns->registerClass("ext_ClassSubA", typeid(ClassSubA));
		cls = ns->getClass("ext_ClassSubA");
		cls->begin();
		cls->extends<ClassSubA, ClassSupperA, ClassSupperB>();
		cls->registerFunction(&ClassSubA::takeAShower, "takeAShower");
		cls->registerFunction(SimpleCreate<ClassSubA>::func, "simpleCreate");
		cls->end();
		

		ns->registerClass("ext_ClassSubB", typeid(ClassSubB));
		cls = ns->getClass("ext_ClassSubB");
		cls->begin();
		cls->extends<ClassSubB, ClassSupperB>();
		cls->registerFunction(&ClassSubB::getMyCat, "getMyCat");
		cls->registerFunction(&ClassSubB::getMyGirlfriend, "getMyGirlfriend");
		cls->registerFunction(SimpleCreate<ClassSubB>::func, "simpleCreate");
		cls->end();

		ns->registerClass("ext_ClassSubC", typeid(ClassSubC));
		cls = ns->getClass("ext_ClassSubC");
		cls->begin();
		cls->extends<ClassSubC, ClassSubA>();
		cls->registerFunction(&ClassSubC::takeABreak, "takeABreak");
		cls->registerFunction(SimpleCreate<ClassSubC>::func, "simpleCreate");
		cls->end();
	}
	ns->end();
	lae.end();

	auto L = lae.getLua_State();
	int const loadStatus = luaL_loadfile(L, "LuaAdapter/testLuaSrc/extends_test.lua");
	if (LUA_OK != loadStatus) {
		cout << lua_tostring(L, -1) << endl;
		assert(false);
	}
	lua_call(L, 0, 0);
}
