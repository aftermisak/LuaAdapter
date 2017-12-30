#include "Class.hpp"
#include "Namespace.hpp"
#include <cassert>
using namespace LuaAdapter;


Class::Class(lua_State* L, const string& name) :
	_L(L),
	_name(name)
{

}

void Class::begin(){
	//a namespace must on lua stack
	assert(lua_gettop(_L) == 2);
	lua_pushstring(_L, "__classTable__");
	lua_rawget(_L, -2); //top is __classTable__ now

	lua_pushstring(_L, _name.c_str());
	lua_rawget(_L, -2);//top is class now

	assert(lua_istable(_L, -1));
}



void Class::end(){
	assert(lua_gettop(_L) == 4);//luaAdapter, namespace, __classTable__, class

	//pop class and __classTable__ of namespace
	lua_pop(_L, 2);
}

void Class::extends(const type_info& clsInfo) {
	const string className = Namespace::getClassNameByTypeInfo(clsInfo);
	assert( className != "" );
	extends(className);
}

void Class::extends( const string& nameOfClass){
	assert(lua_gettop(_L) == 4);//luaAdapter, namespace, __classTable__, class

	lua_pushstring(_L, "__supperClasses__");
	lua_rawget(_L, -2);
	//for get next supper class index
	int supperCount = la_lua_table_len(_L, -1);
	int nextSupperIndex = supperCount + 1;

	lua_pushinteger(_L, nextSupperIndex);
	lua_getglobal(_L, "luaAdapter_lua_getClassTable");
	assert(lua_isfunction(_L, -1));
	lua_pushstring(_L, nameOfClass.c_str());
	lua_pcall(_L, 1, 1, 0);
	lua_rawset(_L, -3);

	lua_pop(_L, 1);//pop __supperClasses__ table

	assert(lua_gettop(_L) == 4);//luaAdapter, namespace, __classTable__, class

}