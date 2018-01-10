#pragma once


#include <iostream>
#include <type_traits>
#include <cassert>
#include <string>

#include "adapter_lua.hpp"
#include "return_count.hpp"
#include "param_count.hpp"
#include "lua_param.hpp"
#include "push_to_lua.hpp"
using namespace std;

namespace LuaAdapter{

	
#pragma region CallImp
	template<typename Return, typename ParmList>
	struct CallImp;

	template<typename Return>
	struct CallImp<Return, LuaParmList<>>
	{
		template<typename T, typename Fun, typename ParmList>
		inline static Return callMember(T* obj, Fun f, const ParmList& parmList){
			return (obj->*f)();
		}

		template<typename Fun, typename ParmList>
		inline static Return callStatic(Fun f, const ParmList& parmList){
			return (*f)();
		}
	};

	template<typename Return, typename Parm1>
	struct CallImp<Return, LuaParmList<Parm1>>
	{
		template<typename T, typename Fun, typename ParmList>
		inline static Return callMember(T* obj, Fun f, const ParmList& parmList){
			return (obj->*f)(parmList.head.value);
		}

		template<typename Fun, typename ParmList>
		inline static Return callStatic(Fun f, const ParmList& parmList){
			return (*f)(parmList.head.value);
		}
	};

	template<typename Return, typename Parm1, typename Parm2>
	struct CallImp<Return, LuaParmList<Parm1, Parm2>>
	{
		template<typename T, typename Fun, typename ParmList>
		inline static Return callMember(T* obj, Fun f, const ParmList& parmList){
			return (obj->*f)(parmList.head.value, parmList.others.head.value);
		}
		template<typename Fun, typename ParmList>
		inline static Return callStatic(Fun f, const ParmList& parmList){
			return (*f)(parmList.head.value, parmList.others.head.value);
		}
	};

	template<typename Return, typename Parm1, typename Parm2, typename Parm3>
	struct CallImp<Return, LuaParmList<Parm1, Parm2, Parm3>>
	{
		template<typename T, typename Fun, typename ParmList>
		inline static Return callMember(T* obj, Fun f, const ParmList& parmList) {
			return (obj->*f)(parmList.head.value, parmList.others.head.value, parmList.others.others.head.value);
		}
		template<typename Fun, typename ParmList>
		inline static Return callStatic(Fun f, const ParmList& parmList) {
			return (*f)(parmList.head.value, parmList.others.head.value, parmList.others.others.head.value);
		}
	};

	template<typename Return, typename Parm1, typename Parm2, typename Parm3, typename Parm4>
	struct CallImp<Return, LuaParmList<Parm1, Parm2, Parm3, Parm4>>
	{
		template<typename T, typename Fun, typename ParmList>
		inline static Return callMember(T* obj, Fun f, const ParmList& parmList) {
			return (obj->*f)(parmList.head.value, parmList.others.head.value, parmList.others.others.head.value, parmList.others.others.others.head.value);
		}
		template<typename Fun, typename ParmList>
		inline static Return callStatic(Fun f, const ParmList& parmList) {
			return (*f)(parmList.head.value, parmList.others.head.value, parmList.others.others.head.value, parmList.others.others.others.head.value);
		}
	};
	template<typename Return, typename Parm1, typename Parm2, typename Parm3, typename Parm4, typename Param5>
	struct CallImp<Return, LuaParmList<Parm1, Parm2, Parm3, Parm4, Param5>>
	{
		template<typename T, typename Fun, typename ParmList>
		inline static Return callMember(T* obj, Fun f, const ParmList& parmList) {
			return (obj->*f)(parmList.head.value, parmList.others.head.value, parmList.others.others.head.value, parmList.others.others.others.head.value,
				parmList.others.others.others.others.head.value);
		}
		template<typename Fun, typename ParmList>
		inline static Return callStatic(Fun f, const ParmList& parmList) {
			return (*f)(parmList.head.value, parmList.others.head.value, parmList.others.others.head.value, parmList.others.others.others.head.value, 
				parmList.others.others.others.others.head.value);
		}
	};
	template<typename Return, typename Parm1, typename Parm2, typename Parm3, typename Parm4, typename Param5, typename Param6>
	struct CallImp<Return, LuaParmList<Parm1, Parm2, Parm3, Parm4, Param5, Param6>>
	{
		template<typename T, typename Fun, typename ParmList>
		inline static Return callMember(T* obj, Fun f, const ParmList& parmList) {
			return (obj->*f)(parmList.head.value, parmList.others.head.value, parmList.others.others.head.value, parmList.others.others.others.head.value,
				parmList.others.others.others.others.head.value, parmList.others.others.others.others.others.head.value);
		}
		template<typename Fun, typename ParmList>
		inline static Return callStatic(Fun f, const ParmList& parmList) {
			return (*f)(parmList.head.value, parmList.others.head.value, parmList.others.others.head.value, parmList.others.others.others.head.value,
				parmList.others.others.others.others.head.value, parmList.others.others.others.others.others.head.value);
		}
	};
	template<typename Return, typename Parm1, typename Parm2, typename Parm3, typename Parm4, typename Param5, typename Param6, typename Param7>
	struct CallImp<Return, LuaParmList<Parm1, Parm2, Parm3, Parm4, Param5, Param6, Param7>>
	{
		template<typename T, typename Fun, typename ParmList>
		inline static Return callMember(T* obj, Fun f, const ParmList& parmList) {
			return (obj->*f)(parmList.head.value, parmList.others.head.value, parmList.others.others.head.value, parmList.others.others.others.head.value,
				parmList.others.others.others.others.head.value, parmList.others.others.others.others.others.head.value, 
				parmList.others.others.others.others.others.others.head.value);
		}
		template<typename Fun, typename ParmList>
		inline static Return callStatic(Fun f, const ParmList& parmList) {
			return (*f)(parmList.head.value, parmList.others.head.value, parmList.others.others.head.value, parmList.others.others.others.head.value,
				parmList.others.others.others.others.head.value, parmList.others.others.others.others.others.head.value, 
				parmList.others.others.others.others.others.others.head.value);
		}
	};

	//you need more?

#pragma endregion

	template<typename Fun>
	struct Call;

	//implementation of member
	template<typename R, typename T, typename ...TParms>
	struct Call<R(T::*)(TParms...)>{
		typedef R(T::* Fun)(TParms...);
		typedef LuaParmList<T*, TParms...> ParmList;
		typedef LuaParmList<TParms...> RelParmList;

		static void call(lua_State* L, Fun f){
			_call(L, f, typename std::is_void<R>::type());
		}
	private:
		typedef std::true_type IsVoidReturn;
		typedef std::false_type NotVoidReturn;
		static void _call(lua_State*L, Fun f, IsVoidReturn){
			ParmList parmList(L, -(int)sizeof...(TParms)-1);
			CallImp<R, RelParmList>::callMember(parmList.head.value, f, parmList.others);
		}

		static void _call(lua_State*L, Fun f, NotVoidReturn){
			ParmList parmList(L, -(int)sizeof...(TParms)-1);
			PushToLua<R>::push(L, CallImp<R, RelParmList>::callMember(parmList.head.value, f, parmList.others));
		}
	};

	//implementation of const member
	template<typename R, typename T, typename ...TParms>
	struct Call<R(T::*)(TParms...)const> {
		typedef R(T::* Fun)(TParms...)const;
		typedef LuaParmList<T*, TParms...> ParmList;
		typedef LuaParmList<TParms...> RelParmList;

		static void call(lua_State* L, Fun f) {
			_call(L, f, typename std::is_void<R>::type());
		}
	private:
		typedef std::true_type IsVoidReturn;
		typedef std::false_type NotVoidReturn;
		static void _call(lua_State*L, Fun f, IsVoidReturn) {
			ParmList parmList(L, -(int)sizeof...(TParms)-1);
			CallImp<R, RelParmList>::callMember(parmList.head.value, f, parmList.others);
		}

		static void _call(lua_State*L, Fun f, NotVoidReturn) {
			ParmList parmList(L, -(int)sizeof...(TParms)-1);
			PushToLua<R>::push(L, CallImp<R, RelParmList>::callMember(parmList.head.value, f, parmList.others));
		}
	};

	//implemetation for static
	template<typename R, typename ...TParms>
	struct Call< R(*)(TParms...) >{
		typedef R(*Fun)(TParms...);
		typedef LuaParmList<TParms...> ParmList;

		static void  call(lua_State * L, Fun f){
			_call(L, f, typename std::is_void<R>::type());
		}

	private:
		typedef std::true_type IsVoidReturn;
		typedef std::false_type NotVoidReturn;
		static void _call(lua_State*L, Fun f, IsVoidReturn){
			ParmList parmList(L, -(int)sizeof...(TParms));
			CallImp<R, ParmList>::callStatic(f, parmList);
		}

		static void _call(lua_State*L, Fun f, NotVoidReturn){
			ParmList parmList(L, -(int)sizeof...(TParms));
			PushToLua<R>::push(L, CallImp<R, ParmList>::callStatic(f, parmList));
		}
	};


	template <typename Fun>
	struct ToLuaFunc{
		static int fun(lua_State* L){

			void* userdata = lua_touserdata(L, lua_upvalueindex(1));
			Fun  fp = *static_cast<Fun*>(userdata);

			const unsigned paramCount = ParmCount<Fun>::result;
			const unsigned stackCount = (unsigned)lua_gettop(L);
			if (std::is_member_function_pointer<Fun>::value){
				if (paramCount != stackCount - 1){
					luaL_error(L, "the member function parm count is %d, but get count %d.", paramCount, stackCount - 1);
					return 0;
				}
			}
			else{
				if (paramCount != stackCount){
					luaL_error(L, "the static function parm count is %d, but get count %d.", paramCount, stackCount);
					return 0;
				}
			}

			Call<Fun>::call(L, fp);

			return ReturnCount<Fun>::result;
		}
	};

	template<typename EType>
	struct EnumToIntType {
		static_assert(std::is_enum<EType>::value, "logic error");
		template< int Size >
		struct SizeToInt;

		template<> struct SizeToInt<4> {
			typedef int32_t type;
		};

		template<> struct SizeToInt<8> {
			typedef int64_t type;
		};

		typedef typename SizeToInt<sizeof(EType)>::type type;
	};

	template<typename T>
	struct EnumTypeToIntIfNeed {
		template<typename T, typename TOrF> struct _ACast;
		template<typename T> struct _ACast<T, std::true_type> { typedef typename EnumToIntType<T>::type type; };
		template<typename T> struct _ACast<T, std::false_type> { typedef T type; };
	
		typedef typename _ACast<T, typename std::is_enum<T>::type>::type type;
	};

	template<typename Pointer>
	struct ToLuaDataGet;

	template<typename T>
	struct ToLuaDataGet<T*>{
		typedef typename EnumTypeToIntIfNeed<T>::type T;
		static int fun(lua_State* L){
			void* userdata = lua_touserdata(L, lua_upvalueindex(1));
			T** p = static_cast<T**>(userdata);
			const T& value = **p;
			//remove const because I don't want implement const version for push
			PushToLua< typename std::remove_const<T>::type >::push(L, value);
			return 1;
		}
	};

	template<typename T, typename C>
	struct ToLuaDataGet<T C::*>{
		typedef typename EnumTypeToIntIfNeed<T>::type T;
		static int fun(lua_State* L){
			void* userdata = lua_touserdata(L, lua_upvalueindex(1));
			T C::** p = static_cast<T C::**>(userdata);
			LuaParm<C*> obj(L, -1);
			const T& value = obj.value->**p;
			PushToLua<typename std::remove_const<T>::type>::push(L, value);
			return 1;
		}
	};

	template<typename Pointer>
	struct ToLuaDataSet;

	//static var partial specialization
	template<typename T>
	struct ToLuaDataSet<T*>{
		typedef typename EnumTypeToIntIfNeed<T>::type T;
		static int fun(lua_State* L){

			if (std::is_const<T>::value){
				luaL_error(L, "the data is read only");
				return 0;
			}

			const unsigned stackCount = (unsigned)lua_gettop(L);
			if (1 != stackCount){
				luaL_error(L, "the data set take and only take one parm");
			}

			void* userdata = lua_touserdata(L, lua_upvalueindex(1));
			T** p = static_cast<T**>(userdata);

			LuaParm<T> parm(L, -1);
			//const_cast<typename std::remove_const<T>::type&>(**p) = parm.value;//for compile const
			**p = parm.value;
			return 0;
		}
	};
	template<typename T>
	struct ToLuaDataSet<const T*> {
		inline static int fun(lua_State* L) {
			luaL_error(L, "this data is read only");
			return 0;
		}
	};

	//member var partial specialization
	template<typename T, typename C>
	struct ToLuaDataSet<T C::*>{
		typedef typename EnumTypeToIntIfNeed<T>::type T;
		static int fun(lua_State* L){

			if (std::is_const<T>::value){
				luaL_error(L, "the data is read only");
				return 0;
			}

			const unsigned stackCount = (unsigned)lua_gettop(L);
			if (2 != stackCount){
				luaL_error(L, "the member data set take two parm");
			}

			void* userdata = lua_touserdata(L, lua_upvalueindex(1));
			T C::** p = static_cast<T C::**>(userdata);

			LuaParm<C*> parm1(L, -2);
			LuaParm<T> parm2(L, -1);
			C* obj = parm1.value;
			//const_cast<typename std::remove_const<T>::type&>(obj->**p) = parm2.value;//for compile const
			obj->**p = parm2.value;
			return 0;

		}
	};
	template<typename T, typename C>
	struct ToLuaDataSet<const T C::*> {
		inline static int fun(lua_State* L) {
			luaL_error(L, "this data is read only");
			return 0;
		}
	};

	static void nameCheck(lua_State* L, const string& name) {
#ifdef _DEBUG
		lua_pushstring(L, name.c_str());
		lua_rawget(L, -2);
		assert(lua_isnil(L, -1));
		lua_pop(L, 1);
#endif // _DEBUG
	}

	template <typename TFunc>
	void doRegisterFunction(lua_State * L, TFunc fun, const string& name){

		nameCheck(L, name);

		//register function
		lua_pushstring(L, name.c_str());
		new (lua_newuserdata(L, sizeof(TFunc))) TFunc(fun);
		lua_pushcclosure(L, ToLuaFunc<TFunc>::fun, 1);
		lua_rawset(L, -3);

	}

	template<typename Pointer>
	static void _doRegisterVariableGet(lua_State*L, Pointer pData, const string& name){
		nameCheck(L, name);

		lua_pushstring(L, name.c_str());
		new (lua_newuserdata(L, sizeof(Pointer))) Pointer(pData);
		lua_pushcclosure(L, ToLuaDataGet<Pointer>::fun, 1);

		lua_rawset(L, -3);
	}

	template<typename T>
	inline void doRegisterVariableGet(lua_State* L, T* data, const string& name){
		_doRegisterVariableGet(L, data, name);

	}

	template<typename T, typename C>
	inline void doRegisterVariableGet(lua_State* L, T C::* data, const string& name){
		_doRegisterVariableGet(L, data, name);
	}

	template<typename Pointer>
	static void _doRegisterVariableSet(lua_State*L, Pointer pData, const string& name){
		nameCheck(L, name);
		lua_pushstring(L, name.c_str());
		new (lua_newuserdata(L, sizeof(Pointer))) Pointer(pData);
		lua_pushcclosure(L, ToLuaDataSet<Pointer>::fun, 1);
		lua_rawset(L, -3);
	}

	template<typename T>
	inline void doRegisterVariableSet(lua_State*L, T* data, const string& name){
		_doRegisterVariableSet(L, data, name);
	}

	template<typename T, typename C>
	inline void doRegisterVariableSet(lua_State*L, T C::* data, const string& name){
		_doRegisterVariableSet(L, data, name);
	}

}