#include "Namespace.hpp"
#include "Class.hpp"
using namespace LuaAdapter;

static const string lua_adapter_namespace_token = "hidden in dark";
std::unordered_map<Namespace::class_hash, string > Namespace::__clsTypeNameMap;

Namespace::Namespace(lua_State* L, const string& name) :
_L(L), _name(name)
{

}


void Namespace::begin(){
	//lua  adapter must the only on stack
	assert(lua_gettop(_L) == 1);

	lua_pushstring(_L, _name.c_str());
	lua_rawget(_L, -2);
}
void Namespace::end(){

	assert(lua_gettop(_L) == 2);

	lua_pushstring(_L, "luaAdapterNamespaceToken");

	lua_rawget(_L, -2);

	const string tokenValue = lua_tostring(_L, -1);
	assert(lua_adapter_namespace_token == tokenValue);

	lua_pop(_L, 2);

	assert(lua_gettop(_L) == 1);
}

void Namespace::registerClass(const string& name, const type_info& clsInfo){
	assert(!hasClass(name));

	lua_pushstring(_L, "__classTable__");
	lua_rawget(_L, -2);

	lua_pushstring(_L, name.c_str());
	//create class 
	lua_getglobal(_L, "luaAdapter_lua_createClassTable");
	lua_pushstring(_L, name.c_str());
	const auto callStatu = lua_pcall(_L, 1, 1, 0);
	if (callStatu != LUA_OK){
		const char* errorMsg = lua_tostring(_L, -1);
		luaL_error(_L, errorMsg);
		assert(false);
	}
	assert(lua_istable(_L, -1));
	lua_rawset(_L, -3);

	lua_pop(_L, 1);//pop__classTable__
	
	__classNames.insert(name);
	__clsTypeNameMap.insert(make_pair(clsInfo.hash_code(), name));
}

shared_ptr<Class> Namespace::getClass(const string& name){
	return make_shared<Class>(_L, name);
}

string Namespace::getClassNameByTypeInfo(const type_info& clsInfo) {
	auto iter = __clsTypeNameMap.find(clsInfo.hash_code());
	if (iter == __clsTypeNameMap.end()) { return ""; }
	return iter->second;
}