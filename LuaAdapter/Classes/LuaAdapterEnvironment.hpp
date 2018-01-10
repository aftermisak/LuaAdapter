#pragma once

#include "adapter_lua.hpp"
#include <memory>
#include <string>
#include <unordered_set>
using namespace std;

namespace LuaAdapter{

	class Namespace;
	class LuaAdapterEnvironment{

	public:
		static LuaAdapterEnvironment& getInstance();

		void init(lua_State* L);

		inline lua_State* getLua_State() {
			return _L;
		}

		std::shared_ptr<Namespace> getNamespace(const string& name);
		void registerNamespace(const string& name);

		void begin();
		void end();

		typedef lua_Integer LuaFunctionId;
		LuaFunctionId retainLuaFunction(int index);
		void releaseLuaFunction(LuaFunctionId lfId);
		void getLuaFunctionById(LuaFunctionId lfId);

	private:
		bool hasNamespace(const string& name);
		inline LuaFunctionId nextLuaFunctionId() { return ++__nextLuaFId; }
		static inline const char* lua_function_map_field_name(){
			return "lua_adapter_lua_function_map_field_name";
		}
	private:
		lua_State* _L;
		LuaAdapterEnvironment();
		~LuaAdapterEnvironment();
		LuaAdapterEnvironment(const LuaAdapterEnvironment&) = delete;
		LuaAdapterEnvironment& operator=(const LuaAdapterEnvironment&) = delete;

		std::unordered_set<string> _namespaceNames;

		LuaFunctionId __nextLuaFId = 0;
		std::unordered_set<LuaFunctionId> __luaFRecord;
	};

}