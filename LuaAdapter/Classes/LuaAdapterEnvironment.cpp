#include "LuaAdapterEnvironment.hpp"

#include <cassert>
#include <string>
#include <iostream>
#include "Namespace.hpp"

using namespace LuaAdapter;

static const string lua_adapter_token = "lcx looking for you";

LuaAdapterEnvironment::LuaAdapterEnvironment():
_L(nullptr){

}
LuaAdapterEnvironment::~LuaAdapterEnvironment(){
	
}

LuaAdapterEnvironment& LuaAdapterEnvironment::getInstance() {
	static LuaAdapterEnvironment _instance;
	return _instance;
}

void LuaAdapterEnvironment::init(lua_State* L){
	auto stackCount = lua_gettop(L);
	assert(L);
	assert(_L == nullptr);
	_L = L;
	luaL_dofile(L, "LuaAdapter/luaApdaterLuaSrc/luaAdapter.lua");
	luaL_dofile(L, "LuaAdapter/luaApdaterLuaSrc/namespace.lua");
	luaL_dofile(L, "LuaAdapter/luaApdaterLuaSrc/class.lua");

	auto stackCountAfterLoad = lua_gettop(L);
	if (stackCount != stackCountAfterLoad){
		const char* str = lua_tostring(L, -1);
		std::cout << "lua adapter init error:" << std::endl;
		std::cout << str << std::endl;
	}

	//init lua function ref table
	lua_pushstring(_L, lua_function_map_field_name());
	lua_newtable(_L);
	lua_rawset(_L, LUA_REGISTRYINDEX);
}

shared_ptr<Namespace> LuaAdapterEnvironment::getNamespace(const string& name){

	return  hasNamespace(name) ?
		shared_ptr<Namespace>(new Namespace(_L, name)) : nullptr;
}
void LuaAdapterEnvironment::registerNamespace(const string& name){

	assert(!hasNamespace(name));// can't repeate

	lua_pushstring(_L, name.c_str());
	lua_getglobal(_L, "luaAdapter_lua_createNamespace");
	assert(lua_isfunction(_L, -1));
	const auto callStatu = lua_pcall(_L, 0, 1, 0);
	if (callStatu != LUA_OK){
		const string errorMsg = lua_tostring(_L, -1);
		assert(false);
	}
	assert(lua_istable(_L, -1));
	lua_rawset(_L, -3);

	_namespaceNames.insert(name.c_str());
}


bool LuaAdapterEnvironment::hasNamespace(const string& name){
	auto iter = _namespaceNames.find(name);
	if (iter == _namespaceNames.end()){
		return false;
	}

	return true;
}

void LuaAdapterEnvironment::begin(){
	assert(lua_gettop(_L) == 0);
	lua_getglobal(_L, "luaAdapter");
}
void LuaAdapterEnvironment::end(){
	assert(lua_gettop(_L) == 1);

	lua_pushstring(_L, "luaAdapterToken");

	lua_rawget(_L, -2);
	const string tokenValue = lua_tostring(_L, -1);
	assert(tokenValue == lua_adapter_token);

	//pop the token and luaAdapter
	lua_pop(_L, 2);

	assert(lua_gettop(_L) == 0);

}


LuaAdapterEnvironment::LuaFunctionId LuaAdapterEnvironment::retainLuaFunction(int index) {
	assert(lua_isfunction(_L, index));
	auto id = nextLuaFunctionId();
	lua_pushstring(_L, lua_function_map_field_name());/* stack:... str */
	lua_rawget(_L, LUA_REGISTRYINDEX); /* stack:... refTable */
	assert(lua_istable(_L, -1));
	lua_pushinteger(_L, id); /* stack:... refTable, fId  */
	lua_pushvalue(_L, index-2);
	assert(lua_isfunction(_L, -1));
	lua_rawset(_L, -3);
	lua_pop(_L, 1);

	assert(__luaFRecord.find(id) == __luaFRecord.end());
	__luaFRecord.insert(id);

	return id;
}

void LuaAdapterEnvironment::getLuaFunctionById(LuaFunctionId lfId) {
	lua_pushstring(_L, lua_function_map_field_name()); /* stack: ... str */
	lua_rawget(_L, LUA_REGISTRYINDEX);  /* stack: ... refTable */
	assert(lua_istable(_L, -1));
	lua_pushinteger(_L, lfId); /* stack:... refTable lfId */
	lua_rawget(_L, -2);  /* stack:... refTable func */
	assert(lua_isfunction(_L, -1));
	lua_remove(_L, -2);
}
void LuaAdapterEnvironment::releaseLuaFunction(LuaFunctionId lfId) {
	if (__luaFRecord.find(lfId) == __luaFRecord.end()) {
		return;
	}
	lua_pushstring(_L, lua_function_map_field_name());
	lua_rawget(_L, LUA_REGISTRYINDEX);
	lua_pushinteger(_L, lfId);
	lua_pushnil(_L);
	lua_rawset(_L, -3);
	lua_pop(_L, 1);

	__luaFRecord.erase(lfId);
}