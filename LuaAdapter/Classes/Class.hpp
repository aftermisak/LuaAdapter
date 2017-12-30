#pragma once
#include <type_traits>
#include <cassert>
#include <string>
#include "RegisterImplementation.hpp"

namespace LuaAdapter{

	struct ObjUserData{
		void* obj = nullptr;
	};

	class Class{

	public:
		Class(lua_State*, const string& name);

		void begin();

		void end();

		void extends(const string& nameOfClass);
		void extends(const type_info& clsInfo );

		//template<typename T, typename ...Parm>
		//void registerConstructor(void(*fp)(T*, Parm...), const string& name);

		//template<typename T>
		//void registerDestructor(void(*fp)(T*));

		template<typename Fun>
		void registerFunction(Fun fun, const string& name);

		template<typename T>
		void registerVariable(T* data, const string& name);// static version

		template<typename T, typename C>
		void registerVariable(T C::* data, const string& name);//member  version

	private:

		template<typename Pointer>
		void _registerVariable(Pointer pData, const string& name);

		typedef std::true_type IsMember;
		typedef std::false_type NotMember;

		static inline string _getFunctionTableName(IsMember){
			return "__FuncMemberTable__";
		}
		static inline string _getFunctionTableName(NotMember){
			return "__staticFuncTable__";
		}

		static inline string _getDataGetTableName(IsMember){
			return "__DataMemberGetTable__";
		}
		static inline string _getDataGetTableName(NotMember){
			return "__DataGetTable__";
		}

		static inline string _getDataSetTableName(IsMember){
			return "__DataMemberSetTable__";
		}
		static inline string _getDataSetTableName(NotMember){
			return "__DataSetTable__";
		}


	private:
		lua_State* _L;
		const string _name;
	};


	template<typename Fun>
	void Class::registerFunction(Fun fun, const string& name){
		const string funcTableName = _getFunctionTableName(typename std::is_member_function_pointer<Fun>::type());
		lua_pushstring(_L, funcTableName.c_str());
		lua_rawget(_L, -2);
		doRegisterFunction(_L, fun, name);
		lua_pop(_L, 1);//pop the function table
	}

	template<typename Pointer>
	void Class::_registerVariable(Pointer pData, const string& name){
		const string getTableName = _getDataGetTableName(typename std::is_member_pointer<Pointer>::type());
		lua_pushstring(_L, getTableName.c_str());
		lua_rawget(_L, -2);
		doRegisterVariableGet(_L, pData, name);
		lua_pop(_L, 1);//pop data get table;


		const string setTableName = _getDataSetTableName(typename std::is_member_pointer<Pointer>::type());
		lua_pushstring(_L, setTableName.c_str());
		lua_rawget(_L, -2);
		doRegisterVariableSet(_L, pData, name);
		lua_pop(_L, 1);//pop data set table
	}

	template<typename T>
	void Class::registerVariable(T* data, const string& name){

		this->_registerVariable(data, name);

	}
	template<typename T, typename C>
	void Class::registerVariable(T C::* data, const string& name){
		this->_registerVariable(data, name);
	}

//
//	template<typename Fun>
//	struct ToLuaConstructor;
//
//	template<typename T, typename ...Parm>
//	struct ToLuaConstructor<void(*)(T*, Parm...)>{
//		typedef void(*Fun)(T*, Parm...);
//
//		static int fun(lua_State* L){
//			void* userdata = lua_touserdata(L, lua_upvalueindex(1));
//			Fun  fp = *static_cast<Fun*>(userdata);
//
//			const int realParmCount = sizeof...(Parm);
//			const int parmCount = lua_gettop(L);
//			if (realParmCount != parmCount){
//				luaL_error(L, "parm count not match");
//			}
//
//			ObjUserData* objUserData = (ObjUserData*)lua_newuserdata(L, sizeof(ObjUserData));
//			objUserData->obj = malloc(sizeof(T)); //instance, TO DO
//			lua_insert(L, -realParmCount-1); //obj as first param
//			Call<Fun>::call(L, fp);// call constructor
//			lua_pop(L, realParmCount);//pop constructor params
//
//			std::string className = Namespace::getClassNameByTypeInfo(typeid(std::remove_reference<std::remove_cv<T>::type>::type));
//			lua_getglobal(L, "luaAdapter_lua_getObjectMetatable");
//			lua_pushstring(L, className.c_str());
//			lua_call(L, 1, 1);
//			assert(lua_istable(L, -1));//is obj metatable
//			lua_setmetatable(L, -2);// stack: userdata
//
//#ifdef _DEBUG
//			int n = lua_gettop(L);
//#endif
//			return 1;
//		}
//
//	};
//
//	template<typename Fun>
//	struct ToLuaDestructor;
//
//	template<typename T>
//	struct ToLuaDestructor< void(*)(T*) >{
//		typedef void(*Fun)(T*);
//
//		static int fun(lua_State* L){
//			void* userdata = lua_touserdata(L, lua_upvalueindex(1));
//			Fun  fp = *static_cast<Fun*>(userdata);
//
//			const auto parmCount = lua_gettop(L);
//			assert(1 == parmCount && "parm count not match");
//			Call<Fun>::call(L, fp);
//			ObjUserData* objUserData = (ObjUserData*)lua_touserdata(L, -1);
//			free(objUserData->obj);//free instance TO DO
//			return 0;
//		}
//
//	};
//
//	template<typename T, typename ...Parm>
//	void Class::registerConstructor(void(*fp)(T*, Parm...), const string& name){
//		typedef void(*Fun)(T*, Parm...);
//		lua_pushstring(_L, "__Constructors__");
//		lua_rawget(_L, -2);
//
//		lua_pushstring(_L, name.c_str());
//		new (lua_newuserdata(_L, sizeof(Fun))) Fun(fp);
//		lua_pushcclosure(_L, ToLuaConstructor<Fun>::fun, 1);
//		lua_rawset(_L, -3);
//
//		lua_pop(_L, 1);//pop the __Constructors__ table
//
//	}
//
//	template<typename T>
//	void Class::registerDestructor(void(*fp)(T*)){
//		typedef void(*Fun)(T*);
//
//		lua_pushstring(_L, "__Destructor__");
//		new (lua_newuserdata(_L, sizeof(Fun))) Fun(fp);
//		lua_pushcclosure(_L, ToLuaDestructor<Fun>::fun, 1);
//		lua_rawset(_L, -3);
//
//	}

}