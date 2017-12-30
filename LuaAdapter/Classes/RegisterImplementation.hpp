#pragma once

#include "adapter_lua.hpp"
#include <iostream>
#include <type_traits>
#include <cassert>
#include <string>
using namespace std;

namespace LuaAdapter{
	template <typename Fun>
	struct ReturnCount;

	/*
	note:
	compiler cant distinguish void(T::*)(...TParms) from Return(T::*)(...TParms)
	*/

	template<typename Return, typename ...TParms>//static
	struct ReturnCount<Return(*)(TParms...)>{
		static const int result = std::is_void<Return>::value ? 0 : 1;
	};

	template<typename Return, typename T, typename ...TParms>//member
	struct ReturnCount<Return(T::*)(TParms...)>{
		static const int result = std::is_void<Return>::value ? 0 : 1;
	};

	template<typename Return, typename T, typename ...TParms>//const member 
	struct ReturnCount<Return(T::*)(TParms...)const> {
		static const int result = std::is_void<Return>::value ? 0 : 1;
	};

	template <typename Fun>
	struct ParmCount;

	/*
	note:
	compiler cant distinguish void(T::*)(...TParms) from Return(T::*)(...TParms)
	*/

	template<typename Return, typename ...TParms>//static
	struct ParmCount<Return(*)(TParms...)>{
		static const int result = sizeof...(TParms);
	};

	template<typename Return, typename T, typename ...TParms>//member
	struct ParmCount<Return(T::*)(TParms...)>{
		static const int result = sizeof...(TParms);
	};

	template<typename Return, typename T, typename ...TParms>//const member
	struct ParmCount<Return(T::*)(TParms...)const> {
		static const int result = sizeof...(TParms);
	};





	template<typename T>
	struct LuaParm;


	/*
	//for Class, Class&, const Class, const Class&
	这里的lua parm，只用于保存参数的暂时数据，以便转发给实际调用，
	当目标参数是userdata时，这里只保留一个引用，绝不会拷贝一份参数
	*/
	//template<typename UserClass>
	//struct LuaParm{
	//	UserClass& value;
	//	LuaParm(lua_State* L, int index){
	//		ObjUserData* userdata = (ObjUserData*)lua_touserdata(L, index);
	//		value = *static_cast<UserClass*>(userdata->obj);
	//	}
	//};

	//for Class*, const Class*
	template<typename UserClass>
	struct LuaParm<UserClass*>{
		UserClass* value;
		LuaParm(lua_State* L, int index){
			ObjUserData* userdata = (ObjUserData*)lua_touserdata(L, index);
			value = static_cast<UserClass*>(userdata->obj);
		}
	};

	//数字类型实现
	/*
	数字版本的实现，都将会拷贝一份参数，因为lua原始数据类型只有by value语义。
	并且不实作无符号类型，并且不建议实作无符号类型版本。
	*/

#define gener_luaparm_for_int(inttype) \
	template<>\
	struct LuaParm<inttype>\
	{\
	inttype value; \
	LuaParm(lua_State* L, int index)\
	{\
	value = (inttype)lua_tointeger(L, index); \
	}\
	}; \

	gener_luaparm_for_int(char)
	gener_luaparm_for_int(unsigned char)
	gener_luaparm_for_int(short)
	gener_luaparm_for_int(unsigned short)
	gener_luaparm_for_int(int)
	gener_luaparm_for_int(unsigned int)
	gener_luaparm_for_int(unsigned long)
	gener_luaparm_for_int(unsigned long long)


		//浮点类型

#define gener_luaparm_for_float(floattype) \
	template<>\
	struct LuaParm<floattype>\
	{\
	floattype value; \
	LuaParm(lua_State* L, int index)\
	{\
	value = (floattype)lua_tonumber(L, index); \
	}\
	}; \

		gener_luaparm_for_float(float)
		gener_luaparm_for_float(double)

		//字符串类型实现
		/*
		字符串类型实作，都将值保存为const char*,
		并且支持cpp定义的字符串类型有const char* , string, const string, const string&
		不支持 char *, string&, string&&

		*/


#define gener_luaparm_for_string(strtype) \
	template<>\
	struct LuaParm<strtype>\
	{\
	const char* value; \
	LuaParm(lua_State* L, int index)\
	{\
	value = (const char*)lua_tostring(L, index); \
	}\
	}; \

		gener_luaparm_for_string(const char*)
		gener_luaparm_for_string(std::string)
		gener_luaparm_for_string(const std::string)
		gener_luaparm_for_string(const std::string&)

	template<>
	struct LuaParm<char*>;
	template<>
	struct LuaParm<std::string&>;
	template<>
	struct LuaParm<std::string&&>;
	
	template<> struct LuaParm<bool> {
		bool value;
		LuaParm(lua_State* L, int index) {
			value = lua_toboolean(L, index) == 0 ? false : true;
		}
	};

	template<typename ...Types>
	struct LuaParmList;

	template<typename H, typename ...TOthers>
	struct LuaParmList<H, TOthers...>{
		LuaParm<H> head;
		LuaParmList<TOthers...> others;

		LuaParmList(lua_State* L, int indexOfFirst) :
			head(L, indexOfFirst),
			others(L, indexOfFirst + 1)
		{
		}
	};

	template<>
	struct LuaParmList<>{
		LuaParmList(lua_State* L, int indexOfFirst){
			//do nothing, only for compile
		}
	};

	//////////////////////////////////////////

	template<typename T>
	struct PushToLua;

	template<typename UserClass>
	struct PushToLua<UserClass *> {
		static void push(lua_State* L, UserClass* v) {
			ObjUserData* objUserData = (ObjUserData*)lua_newuserdata(L, sizeof(ObjUserData));
			objUserData->obj = v;
			std::string className = Namespace::getClassNameByTypeInfo(typeid(std::remove_reference<std::remove_cv<UserClass>::type>::type));
			lua_getglobal(L, "luaAdapter_lua_getObjectMetatable");
			lua_pushstring(L, className.c_str());
			lua_call(L, 1, 1);
			assert(lua_istable(L, -1));//obj metatable
			lua_setmetatable(L, -2);// stack: userdata
			
		}
	};
	//wait implementation for UserData

	template<>
	struct PushToLua<void>{
		inline static void push(lua_State* L, ...){
			//do nothing
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

	template<>
	struct PushToLua<std::string>{
		inline static void push(lua_State*L, const string& value){
			lua_pushstring(L, value.c_str());
		}
	};
	template<> struct PushToLua<const std::string> : public PushToLua<std::string> {};
	template<> struct PushToLua<const std::string&> : public PushToLua<std::string> {};
	template<>
	struct PushToLua<const char*>{
		inline static void push(lua_State*L, const char* value){
			lua_pushstring(L, value);
		}
	};

	template<>
	struct PushToLua<bool> {
		inline static void push(lua_State*L, bool value) {
			lua_pushboolean(L, value ? 1 : 0);
		}
	};

	//////////////////////////////////////////////////////////
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

			//读出函数指针
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


	template<typename Pointer>
	struct ToLuaDataGet;

	template<typename T>
	struct ToLuaDataGet<T*>{
		static int fun(lua_State* L){
			void* userdata = lua_touserdata(L, lua_upvalueindex(1));
			T** p = static_cast<T**>(userdata);
			const T& value = **p;
			PushToLua<T>::push(L, value);
			return 1;
		}
	};

	template<typename T, typename C>
	struct ToLuaDataGet<T C::*>{
		static int fun(lua_State* L){
			void* userdata = lua_touserdata(L, lua_upvalueindex(1));
			T C::** p = static_cast<T C::**>(userdata);
			LuaParm<C*> obj(L, -1);
			const T& value = obj.value->**p;
			PushToLua<T>::push(L, value);
			return 1;
		}
	};

	template<typename Pointer>
	struct ToLuaDataSet;

	//static var partial specialization
	template<typename T>
	struct ToLuaDataSet<T*>{
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
			const_cast<typename std::remove_const<T>::type&>(**p) = parm.value;//这样是为了兼容对const量的编译
			return 0;
		}
	};

	//member var partial specialization
	template<typename T, typename C>
	struct ToLuaDataSet<T C::*>{
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
			const_cast<typename std::remove_const<T>::type&>(obj->**p) = parm2.value;//这样是为了兼容对const量的编译
			return 0;

		}

	};

	void nameCheck(lua_State* L, const string& name);
	/*
	将函数注册到当前栈的顶端环境
	调用此函数时，栈顶应该是支持index的lua对象，比如table或者userdata
	*/
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