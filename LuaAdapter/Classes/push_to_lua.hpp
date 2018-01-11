#pragma once
#include <string>
#include <type_traits>
#include <cassert>
#include <list>
#include <vector>
#include <unordered_map>
#include "adapter_lua.hpp"

using namespace std;
namespace LuaAdapter {


	template<typename T>
	struct PushToLua;



	template<> struct PushToLua<void> {
		inline static void push(lua_State* L, int) {
			//do noting
		}
	};

#define push_to_lua_integer_imp(inttype)\
	template<>\
	struct PushToLua<inttype>{\
	inline static void push(lua_State*L, inttype value){\
		lua_pushinteger(L, (lua_Integer)value); \
	}\
	};

	push_to_lua_integer_imp(char)
	push_to_lua_integer_imp(unsigned char)
	push_to_lua_integer_imp(short)
	push_to_lua_integer_imp(unsigned short)
	push_to_lua_integer_imp(int)
	push_to_lua_integer_imp(unsigned int)
	push_to_lua_integer_imp(long)
	push_to_lua_integer_imp(unsigned long)
	push_to_lua_integer_imp(unsigned long long)

#define push_to_lua_float_imp(floattype)\
	template<>\
	struct PushToLua<floattype>\
	{\
	inline static void push(lua_State*L, floattype value){\
	lua_pushnumber(L, (lua_Number)value); \
	}\
	};

		push_to_lua_float_imp(float)
		push_to_lua_float_imp(double)

	template<> struct PushToLua<std::string> {
		inline static void push(lua_State*L, const string& value) {
			lua_pushstring(L, value.c_str());
		}
	};
	template<> struct PushToLua<const std::string> : public PushToLua<std::string> {};
	template<> struct PushToLua<const std::string&> : public PushToLua<std::string> {};
	template<> struct PushToLua<const char*> {
		inline static void push(lua_State*L, const char* value) {
			lua_pushstring(L, value);
		}
	};

	template<> struct PushToLua<bool> {
		inline static void push(lua_State*L, bool value) {
			lua_pushboolean(L, value ? 1 : 0);
		}
	};

	template<typename T> struct PushToLua<std::list<T> > {
		static void push(lua_State* L, const std::list<T>& value) {
			lua_newtable(L);
			unsigned i = 1;
			for (auto iter = value.begin() ; iter != value.end()  ; ++i, ++iter) {
				PushToLua<T>::push(L, *iter );
				lua_rawseti(L, -2, int(i));
			}
		}
	};
	template<typename T> struct PushToLua<std::vector<T> > {
		static void push(lua_State* L, const std::vector<T>& value) {
			lua_newtable(L);
			for (unsigned i = 1; i <= value.size(); ++i) {
				PushToLua<T>::push(L, value[value]);
				lua_rawseti(L, -2, int(i));
			}
		}
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename ...Types>
	struct PushToLuaList;

	template<typename TFirst, typename ...TOthers>
	struct PushToLuaList<TFirst, TOthers...> {
		typedef PushToLuaList<TOthers...> SubPushList;
		inline static void push(lua_State* L, TFirst&& first, TOthers&&... others) {
			PushToLua<TFirst>::push(L, std::forward<TFirst>(first));
			SubPushList::push(L, std::forward<TOthers>(others)...);
		}
	};
	template<typename T>
	struct PushToLuaList<T> {
		inline static void push(lua_State* L, T&& v) {
			PushToLua<T>::push(L, std::forward<T>(v));
		}
	};
	template<>
	struct PushToLuaList<> {
		inline static void push(lua_State* L) {}
	};
}
