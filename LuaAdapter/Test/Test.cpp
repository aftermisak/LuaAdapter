#include "Test1.h"
#include "../Classes/LuaAdapter.hpp"
using namespace LuaAdapter;

double staticF1(int pam1, float parm2){

	return pam1 + parm2;
}

static int staticV1 = 1;

class A{

public:

	A(const char* arg1);
	static A* create(const char* arg1);

	void memberF2(int, int){
		std::cout << "memberF2" << std::endl;
	}
	int memberV3;
	static int staticV2;
	static void staticF3(int){
		std::cout << "staticF3" << std::endl;
	}
	~A();
};

class B : public A{
public:
	B(const char* agr);
	static B* create(const char* arg1);

	void memberBF1(  ){
		std::cout << "memberBF1" << std::endl;
	}
	int memberVB1 = 13;

	~B();
};

int A::staticV2 = 1;

A::A(const char* arg1) :
memberV3(2)
{

}


A* A::create( const char* arg1 ){
	return new A(arg1);
}

A::~A(){}

B::B(const char* arg) :A(arg){
}


B* B::create(const char* arg1) {
	return new B(arg1);
}


B::~B(){
}


int fun1(int *){
	return 1;
}

int __luaErrorHook(lua_State* L){

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

void Test1::test1(){
	lua_State * L = luaL_newstate();
	luaL_openlibs(L);
	lua_atpanic(L, __luaErrorHook);


	LuaAdapterEnvironment& la = LuaAdapterEnvironment::getInstance();
	la.init(L);
	la.begin();
	la.registerNamespace("cpp");
	auto ns = la.getNamespace("cpp");
	ns->begin();
	ns->registerFunction(&staticF1, "staticF1");
	ns->registerVariable(&staticV1, "staticV1");

	ns->registerClass("A", typeid(A) );
	auto classA = ns->getClass("A");
	classA->begin();

	classA->registerFunction(A::create, "create");
	classA->registerVariable(&A::staticV2, "staticV2");
	classA->registerVariable(&A::memberV3, "memberV3");
	classA->registerFunction(&A::staticF3, "staticF3");
	classA->registerFunction(&A::memberF2, "memberF2");

	classA->end();

	ns->registerClass("B", typeid(B) );
	auto classB = ns->getClass("B");
	classB->begin();
	classB->extends(typeid(A));
	classB->registerFunction(B::create, "create");
	classB->registerFunction(&B::memberBF1, "memberBF1");
	classB->registerVariable(&B::memberVB1, "memberVB1");
	classB->end();
	ns->end();

	la.end();


	//luaL_dofile(L, "testLuaSrc/main.lua");
	int const loadStatus = luaL_loadfile(L, "LuaAdapter/testLuaSrc/main.lua");
	if (LUA_OK != loadStatus){
		cout << lua_tostring(L, -1) << endl;
		assert(false);

	}
	lua_call(L, 0, 0);
	lua_close(L);
}