#pragma once
#include <string>

#include <list>
#include <vector>
#include <unordered_map>

#include "adapter_lua.hpp"
namespace LuaAdapter {
	template<typename T>
	struct LuaParm;



	template<>
	struct LuaParm<void> {
		LuaParm(lua_State* L, int index){}
	};

	
	template<typename int_type>
	struct LuaParmInt{
		int_type value;
		LuaParmInt(lua_State* L, int index) {
			value = (int_type)luaL_checkinteger(L, index);
		}
	};
	
#define gener_luaparm_for_int(inttype) \
	template<> \
	struct LuaParm<inttype> : public LuaParmInt<std::remove_const<inttype>::type> { \
		using LuaParmInt::LuaParmInt; \
	}; 

	gener_luaparm_for_int(char)
	gener_luaparm_for_int(unsigned char)
	gener_luaparm_for_int(short)
	gener_luaparm_for_int(unsigned short)
	gener_luaparm_for_int(int)
	gener_luaparm_for_int(unsigned int)
	gener_luaparm_for_int(long)
	gener_luaparm_for_int(unsigned long)
	gener_luaparm_for_int(long long)
	gener_luaparm_for_int(unsigned long long)


	template<typename float_type>
	struct LuaParamFloat {
		float_type value;
		LuaParamFloat(lua_State* L, int index) {
			luaL_checktype(L, index, LUA_TNUMBER);
			value = (float_type)lua_tonumber(L, index);
		}
	};
	

#define gener_luaparm_for_float(floattype) \
	template<> \
	struct LuaParm<floattype> : public LuaParamFloat< std::remove_const<floattype>::type > { \
		using LuaParamFloat::LuaParamFloat; \
	}; 

	gener_luaparm_for_float(float)
	gener_luaparm_for_float(double)


	struct LuaParamString {
		const char* value = nullptr;
		LuaParamString(lua_State* L, int index) {
			// value = luaL_checkstring(L, index);//this would cause conversion, like number to string
			luaL_checktype(L, index, LUA_TSTRING);
			value = lua_tostring(L, index);
		}
	};

#define gener_luaparm_for_string(str_type) \
	template<>\
	struct LuaParm<str_type> : public LuaParamString{ \
		using LuaParamString::LuaParamString;\
	};

	gener_luaparm_for_string(const char*)
	gener_luaparm_for_string(std::string)
	gener_luaparm_for_string(const std::string)
	gener_luaparm_for_string(const std::string&)

	
	template<>
	struct LuaParm<char*>;//do not support this
	template<>
	struct LuaParm<std::string&>;//do not support this
	template<>
	struct LuaParm<std::string&&>;//do not support this


	template<> struct LuaParm<bool> {
		bool value;
		LuaParm(lua_State* L, int index) {
			value = lua_toboolean(L, index) == 0 ? false : true;
		}
	};

	

	template<typename T>
	struct LuaParm<std::list<T>>{
		std::list<T> value;
		LuaParm(lua_State* L, int index) {
			luaL_checktype(L, index, LUA_TTABLE);

			const auto array_size = la_lua_table_len(L, index);
			for (unsigned i = 1; i <= array_size; ++i) {
				lua_rawgeti(L, index, int(i)); /* stack :... itemi */
				value.push_back(LuaParm<T>(L, -1).value);
				lua_pop(L, 1);
			}
		}
	};
	template<typename T>
	struct LuaParm<const std::list<T>> : public LuaParm<std::list<T>> {
		using LuaParm<std::list<T>>::LuaParm;
	};
	template<typename T>
	struct LuaParm<const std::list<T>&> : public LuaParm<std::list<T>> {
		using LuaParm<std::list<T>>::LuaParm;
	};

	template<typename T>
	struct LuaParm<std::vector<T>>{
		std::vector<T> value;
		LuaParm(lua_State* L, int index) {
			luaL_checktype(L, index, LUA_TTABLE);

			const auto array_size = la_lua_table_len(L, index);
			value.reserve(array_size);
			for (unsigned i = 1; i <= array_size; ++i) {
				lua_rawgeti(L, index, int(i)); /* stack :... itemi */
				value.push_back(LuaParm<T>(L, -1).value);
				lua_pop(L, 1);
			}
		}
	};
	template<typename T>
	struct LuaParm<const std::vector<T>> : public LuaParm<std::vector<T>> {
		using LuaParm<std::vector<T>>::LuaParm;
	};
	template<typename T>
	struct LuaParm<const std::vector<T>&> : public LuaParm<std::vector<T>> {
		using LuaParm<std::vector<T>>::LuaParm;
	};

	template<typename TKey, typename TValue>
	struct LuaParm< std::unordered_map<TKey, TValue> > {
		std::unordered_map<TKey, TValue> value;
		LuaParm(lua_State* L, int index) {
			luaL_checktype(L, index, LUA_TTABLE);

			const int topn_begin = lua_gettop(L);
			lua_pushnil(L); lua_pushnil(L);  /* nil for first iteration */
			while (lua_next(L, index) != 0) {
				value.insert(std::pair<TKey, TValue>(LuaParm<TKey>(L, -2).value, LuaParm<TValue>(L, -1).value));
				/* removes 'value'; keeps 'key' for next iteration */
				lua_pop(L, 1);
			}
			const int topn_now = lua_gettop(L);
			if( topn_now > topn_begin ){
				lua_pop(L, topn_now - topn_begin);
			}
		}
	};

	template<typename TKey, typename TValue>
	struct LuaParm< const std::unordered_map<TKey, TValue> > : public LuaParm<std::unordered_map<TKey, TValue>> {
		using LuaParm<std::unordered_map<TKey, TValue>>::LuaParm;
	};
	template<typename TKey, typename TValue>
	struct LuaParm< const std::unordered_map<TKey, TValue>& > : public LuaParm<std::unordered_map<TKey, TValue>> {
		using LuaParm<std::unordered_map<TKey, TValue>>::LuaParm;
	};


	template<typename ...Types>
	struct LuaParmList;

	template<typename H, typename ...TOthers>
	struct LuaParmList<H, TOthers...> {
		LuaParm<H> head;
		LuaParmList<TOthers...> others;

		LuaParmList(lua_State* L, int indexOfFirst) :
			head(L, indexOfFirst),
			others(L, indexOfFirst + 1)
		{
		}
	};

	template<>
	struct LuaParmList<> {
		LuaParmList(lua_State* L, int indexOfFirst) {
			//do nothing, only for compile
		}
	};

}