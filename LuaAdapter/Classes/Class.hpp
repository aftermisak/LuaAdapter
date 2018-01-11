#pragma once

#include "RegisterImplementation.hpp"
#include <type_traits>
#include <cassert>
#include <string>
#include <unordered_map>
#include <typeindex>
#include "Namespace.hpp"

namespace LuaAdapter{

	void addOffsetRecord(unsigned h1, unsigned h2, ptrdiff_t offset);
	std::pair<bool, ptrdiff_t> getOffsetRecord(unsigned h1, unsigned h2);
	namespace {
		template<typename Sub, typename ...Suppers>
		struct AddOffsetRecord;

		template<typename Sub, typename Supper, typename ...Suppers>
		struct AddOffsetRecord< Sub, Supper, Suppers...> {
			static void doit() {
				AddOffsetRecord<Sub, Supper>::doit();
				AddOffsetRecord<Sub, Suppers...>::doit();
			}
		};
		template<typename Sub, typename Supper>
		struct AddOffsetRecord< Sub, Supper > {
//            static_assert(std::is_base_of<Supper, Sub>, "error");
			static void doit() {
				const type_info& sub_info = typeid(Sub);
				const type_info& supper_info = typeid(Supper);
				Sub* psub = reinterpret_cast<Sub*>(100000);
				Supper* psup = static_cast<Supper*>(psub);
                
				ptrdiff_t offset_super_to_sub = ptrdiff_t(psup) - ptrdiff_t(psub);
				addOffsetRecord(sub_info.hash_code(), supper_info.hash_code(), offset_super_to_sub);
				addOffsetRecord(supper_info.hash_code(), sub_info.hash_code(), -offset_super_to_sub);
			}
		};
	}

	struct ObjUserData{
		void* obj = nullptr;
		bool needDestruction = false;
		//unsigned type_hash;
		//const char* type_name;
		std::type_index tIndex ;
	};

	template<>
	struct LuaParm<ObjUserData*> {
		ObjUserData* value = nullptr;
		LuaParm(lua_State* L, int index) {
			luaL_checktype(L, index, LUA_TUSERDATA);
			value = (ObjUserData*)lua_touserdata(L, index);
		}
	};

	template<typename UserClass>
	struct LuaParm<UserClass*> {
		UserClass* value;
		//typedef typename std::remove_cv<std::remove_reference<UserClass>::type>::type TheType;
		typedef typename std::decay<UserClass>::type TheType;
		LuaParm(lua_State* L, int index) {
			luaL_checktype(L, index, LUA_TUSERDATA);

			ObjUserData* userdata = (ObjUserData*)lua_touserdata(L, index);
			int offset = 0;
			do{
				const unsigned data_type_hash = userdata->tIndex.hash_code();
				const unsigned type_type_hash = typeid(TheType).hash_code();
				if (data_type_hash == type_type_hash) {//pass for no converision
					break;
				}
				if (data_type_hash != type_type_hash) {
					auto offsetRecord = getOffsetRecord(data_type_hash, type_type_hash);
					if (offsetRecord.first) {
						offset = offsetRecord.second;
					}
					else {
						luaL_error(L, "can't convert userdata at #%d with [%s] to [%s]", index, userdata->tIndex.name(), typeid(TheType).name() );
					}
				}
			} while (false);
			value = reinterpret_cast<UserClass*>( (ptrdiff_t)userdata->obj + offset );
		}
	};

	template<typename UserClass>
	struct PushToLua<UserClass *> {
		//typedef typename std::remove_cv<std::remove_reference<UserClass>::type>::type TheType;
		typedef typename std::decay<UserClass>::type TheType;
		static void push(lua_State* L, UserClass* v) {
			ObjUserData* objUserData = (ObjUserData*)lua_newuserdata(L, sizeof(ObjUserData));
			objUserData->obj = v;
			objUserData->needDestruction = false;
			const type_info& tinfo = typeid(TheType);
			objUserData->tIndex = std::type_index(tinfo);
            std::string className = LuaAdapter::Namespace::getClassNameByTypeInfo(tinfo);
			lua_getglobal(L, "luaAdapter_lua_getObjectMetatable");
			lua_pushstring(L, className.c_str());
			lua_call(L, 1, 1);
			assert(lua_istable(L, -1));//obj metatable
			if (!lua_istable(L, -1)) {
				luaL_error(L, "do not have metatable for class: %s, type_name:%s", className.c_str(), tinfo.name());
			}
			lua_setmetatable(L, -2);// stack: userdata

		}
	};

	class Class{

	public:
		Class(lua_State*, const string& name);

		void begin();

		void end();

		template<typename Sub, typename ...Suppers>
		void extends();

		void extends(const string& nameOfClass);
		void extends(const type_info& clsInfo);

		template<typename T, typename ...Parm>
		void registerConstructor(T*(*fp)(Parm...), const string& name);

		template<typename T>
		void registerDestructor(void(*fp)(T*));

		template<typename Fun>
		void registerFunction(Fun fun, const string& name);

		template<typename T>
		void registerVariable(T* data, const string& name);// static version

		template<typename T, typename C>
		void registerVariable(T C::* data, const string& name);//member  version

		template<typename T>
		void registerEnumValue(T data, const string& name);
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

		std::unordered_map<string, const int*>  __enumTable;

	};
	namespace {
		template<typename ...Suppers>
		struct Class_Extends;

		template<typename Supper1, typename ...Suppers>
		struct Class_Extends<Supper1, Suppers...> {
			static void doit(Class* cls) {
				Class_Extends<Supper1>::doit(cls);
				Class_Extends<Suppers...>::doit(cls);
			}
		};
		template<typename Supper>
		struct Class_Extends<Supper> {
			static void doit(Class* cls) { cls->extends(typeid(Supper)); }
		};
	}

	template<typename Sub, typename ...Suppers>
	void Class::extends() {
		AddOffsetRecord<Sub, Suppers...>::doit();
		Class_Extends<Suppers...>::doit(this);
	}

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

	template<typename T>
	void Class::registerEnumValue(T data, const string& name) {
		static_assert(std::is_enum<T>::value, "logic error");
		__enumTable.insert(make_pair(name, new int( (int)data )));
		const int * refData = __enumTable[name];
		registerVariable(refData, name);
	}


	template<typename Fun>
	struct ToLuaConstructor;


	template<typename T, typename ...Parm>
	struct ToLuaConstructor<T*(*)(Parm...)> {
		typedef T*(*Fun)(Parm...);

		static int fun(lua_State* L) {
			void* userdata = lua_touserdata(L, lua_upvalueindex(1));
			Fun  fp = *static_cast<Fun*>(userdata);

			const int realParmCount = sizeof...(Parm);
			const int parmCount = lua_gettop(L);
			if (realParmCount != parmCount) {
				luaL_error(L, "param count not match");
			}

			Call<Fun>::call(L, fp) ; //call constructor
			ObjUserData* obj = (ObjUserData*)lua_touserdata(L, -1);
			obj->needDestruction = true;
#ifdef _DEBUG
			int n = lua_gettop(L);
#endif
			return 1;
		}

	};

	template<typename Fun>
	struct ToLuaDestructor;

	template<typename T>
	struct ToLuaDestructor< void(*)(T*) >{
		typedef void(*Fun)(T*);

		static int fun(lua_State* L){
			void* userdata = lua_touserdata(L, lua_upvalueindex(1));
			Fun  fp = *static_cast<Fun*>(userdata);

			const auto parmCount = lua_gettop(L);
			if (1 != parmCount) {
				luaL_error(L, "param count not match");
			}
			ObjUserData* objUserData = (ObjUserData*)lua_touserdata(L, -1);
			if (objUserData->needDestruction) {
				Call<Fun>::call(L, fp);
			}
			objUserData->obj = nullptr;
			return 0;
		}

	};

	template<typename T, typename ...Parm>
	void Class::registerConstructor(T*(*fp)(Parm...), const string& name) {
		typedef T*(*Fun)(Parm...);
		lua_pushstring(_L, "__Constructors__");
		lua_rawget(_L, -2);

		lua_pushstring(_L, name.c_str());
		new (lua_newuserdata(_L, sizeof(Fun))) Fun(fp);
		lua_pushcclosure(_L, ToLuaConstructor<Fun>::fun, 1);
		lua_rawset(_L, -3);

		lua_pop(_L, 1);//pop the __Constructors__ table
	}

	template<typename T>
	void Class::registerDestructor(void(*fp)(T*)){
		typedef void(*Fun)(T*);

		lua_pushstring(_L, "__Destructor__");
		new (lua_newuserdata(_L, sizeof(Fun))) Fun(fp);
		lua_pushcclosure(_L, ToLuaDestructor<Fun>::fun, 1);
		lua_rawset(_L, -3);

	}

}
