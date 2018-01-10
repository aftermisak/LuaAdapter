#include "Class.hpp"
#include "Namespace.hpp"
#include <cassert>

using namespace LuaAdapter;

namespace LuaAdapter {
	struct RecordHash {
	private:
		const std::pair<unsigned, unsigned> __arg;
	public:
		explicit RecordHash(const std::pair<unsigned, unsigned>& arg) :__arg(arg){}
		RecordHash(){}

		inline static size_t gen_hash(const std::pair<unsigned, unsigned>& arg ) {
			return (std::hash<unsigned>()(arg.first) << 2) + (std::hash<unsigned>()(arg.second) << 3);
		}
		size_t operator ()() const{
			return gen_hash(__arg);
		}
		size_t operator ()(const std::pair<unsigned, unsigned>& arg) const {
			return gen_hash(arg);
		}
	};

	static std::unordered_multimap< std::pair<unsigned, unsigned>, int, RecordHash > quickOffsetRecord;
}


void LuaAdapter::addOffsetRecord(unsigned h1, unsigned h2, int offset) {
	if (quickOffsetRecord.find(make_pair(h1, h2)) != quickOffsetRecord.end()) {
		return;
	}
	quickOffsetRecord.insert(make_pair(make_pair(h1, h2), offset));
	//inheritance
	for (auto iter = quickOffsetRecord.begin(); iter != quickOffsetRecord.end(); ++iter) {
		if (iter->first.first == h2) {
			const auto more_offset = offset + iter->second;
			addOffsetRecord(h1, iter->first.second, more_offset);
		}
	}

	
}
std::pair<bool, int> LuaAdapter::getOffsetRecord(unsigned h1, unsigned h2) {
	auto iter = quickOffsetRecord.find(make_pair(h1, h2));
	if (iter == quickOffsetRecord.end()) {
		return make_pair(false, 0);
	}
	return make_pair(true, iter->second);
}




static void luaadapter_no_destruction(ObjUserData* objUserData) {
	objUserData->needDestruction = false;
}

Class::Class(lua_State* L, const string& name) :
	_L(L),
	_name(name)
{
	begin();
	registerFunction(&luaadapter_no_destruction, "no_destruction__");
	end();
}

void Class::begin(){
	//a namespace must on lua stack
	assert(lua_gettop(_L) == 2);
	lua_pushstring(_L, "__classTable__");
	lua_rawget(_L, -2); //top is __classTable__ now

	lua_pushstring(_L, _name.c_str());
	lua_rawget(_L, -2);//top is class now

	assert(lua_istable(_L, -1));
}



void Class::end(){
	assert(lua_gettop(_L) == 4);//luaAdapter, namespace, __classTable__, class

	//pop class and __classTable__ of namespace
	lua_pop(_L, 2);
}

void Class::extends(const type_info& clsInfo) {
	const string className = Namespace::getClassNameByTypeInfo(clsInfo);
	assert( className != "" );
	extends(className);
}

void Class::extends( const string& nameOfClass){
	assert(lua_gettop(_L) == 4);//luaAdapter, namespace, __classTable__, class

	lua_pushstring(_L, "__supperClasses__");
	lua_rawget(_L, -2);
	//for get next supper class index
	int supperCount = la_lua_table_len(_L, -1);
	int nextSupperIndex = supperCount + 1;

	lua_pushinteger(_L, nextSupperIndex);
	lua_getglobal(_L, "luaAdapter_lua_getClassTable");
	assert(lua_isfunction(_L, -1));
	lua_pushstring(_L, nameOfClass.c_str());
	lua_pcall(_L, 1, 1, 0);
	lua_rawset(_L, -3);

	lua_pop(_L, 1);//pop __supperClasses__ table

	assert(lua_gettop(_L) == 4);//luaAdapter, namespace, __classTable__, class

}