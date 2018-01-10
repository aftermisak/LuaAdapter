#include "construction_destruction.h"
#include "../Classes/LuaAdapter.hpp"
#include <stdlib.h>
#include <iostream>
namespace {
	class ClassA {
	public:
		ClassA();
		ClassA( unsigned bufflen );
		~ClassA();
		void goHome();
		void goSchool();
	private:
		void* __buffer;
	};
	ClassA::ClassA(unsigned bufflen) {
		__buffer = malloc(bufflen);
	}
	ClassA::ClassA():__buffer(nullptr) {
		__buffer = malloc(8u);
	}

	ClassA::~ClassA() {
		if (__buffer) {
			free(__buffer);
			__buffer = nullptr;
		}
	}
	void ClassA::goHome(){
		std::cout << "A goHome" << std::endl;
	}
	void ClassA::goSchool() {
		std::cout << "A goSchool" << std::endl;
	}

	static ClassA* newA1() {
		return new ClassA;
	}
	static ClassA* newA2( unsigned bl ) {
		return new ClassA(bl);
	}
	static ClassA* createA() {
		return new ClassA;
	}
	static void deleteA( const ClassA* obj) {
		std::cout << "deleteA" << obj << std::endl;
		delete obj;
	}
}

namespace LuaAdapter {

	namespace cons_des_test {

		void test() {
		
			auto& lae = LuaAdapter::LuaAdapterEnvironment::getInstance();
			lae.begin();
			lae.registerNamespace("cons_des_test");
			auto ns = lae.getNamespace("cons_des_test");
			ns->begin();
			{
				ns->registerClass("an_classA", typeid(ClassA));
				auto cls = ns->getClass("an_classA");
				cls->begin(); {
					cls->registerFunction(&ClassA::goHome, "goHome");
					cls->registerFunction(&ClassA::goSchool, "goSchool");
					cls->registerFunction(&createA, "createA");
					cls->registerConstructor(&newA1, "newA1");
					cls->registerConstructor(&newA2, "newA2");
					cls->registerDestructor(&deleteA);
				}
				cls->end();

			}
			ns->end();
			lae.end();
			
			auto L = lae.getLua_State();
			int const loadStatus = luaL_loadfile(L, "LuaAdapter/testLuaSrc/test_construction_destruction.lua");
			if (LUA_OK != loadStatus) {
				cout << lua_tostring(L, -1) << endl;
				assert(false);
			}
			lua_call(L, 0, 0);
		}//end Test

	}

}