#pragma once

#include <string>

#include "adapter_lua.hpp"
#include <cassert>
#include <memory>
#include "RegisterImplementation.hpp"
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>

using namespace std;

namespace LuaAdapter{
	class Class;

	class Namespace{
	public:
		Namespace(lua_State* L, const string& name);

		void begin();

		void end();

		void registerClass(const string& name, const type_info& clsInfo );

		template<typename Fun>
		void registerFunction(Fun fun, const string& name);

		template<typename T>
		void registerVariable(T* var, const string& name);

		shared_ptr<Class> getClass(const string& name);

		static string getClassNameByTypeInfo(const type_info& clsInfo);

		bool hasClass(const string& name) const {
			return __classNames.find(name) != __classNames.end();
		}

	private:
		lua_State* _L;
		const string _name;
		std::unordered_set<string> __classNames;

		typedef size_t class_hash;
		static std::unordered_map<class_hash, string > __clsTypeNameMap;
	};

	template<typename Fun>
	void Namespace::registerFunction(Fun fun, const string& name){

		lua_pushstring(_L, "__staticFuncTable__");
		lua_rawget(_L, -2);
		doRegisterFunction(_L, fun, name);
		lua_pop(_L, 1);
	}

	template<typename T>
	void Namespace::registerVariable(T* var, const string& name){
		lua_pushstring(_L, "__dataGetTable__");
		lua_rawget(_L, -2);
		doRegisterVariableGet(_L, var, name);
		lua_pop(_L, 1);

		lua_pushstring(_L, "__dataSetTable__");
		lua_rawget(_L, -2);
		doRegisterVariableSet(_L, var, name);
		lua_pop(_L, 1);
	}
}