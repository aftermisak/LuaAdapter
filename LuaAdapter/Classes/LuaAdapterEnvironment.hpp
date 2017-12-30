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
		static LuaAdapterEnvironment& getInstance(){

			static LuaAdapterEnvironment _instance;
			return _instance;
		}

		void init(lua_State* L);

		std::shared_ptr<Namespace> getNamespace(const string& name);
		void registerNamespace(const string& name);

		void begin();
		void end();
	private:
		bool hasNamespace(const string& name);

	private:
		lua_State* _L;
		LuaAdapterEnvironment();
		LuaAdapterEnvironment(const LuaAdapterEnvironment&);
		LuaAdapterEnvironment& operator=(const LuaAdapterEnvironment&);

		std::unordered_set<string> _namespaceNames;

	};

}