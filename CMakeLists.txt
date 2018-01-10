project(lua_adapter_test)

include_directories( "LuaAdapter" )

include_directories( "lua/lua-5.1.5/src" )
aux_source_directory( lua/lua-5.1.5/src LUA_SRCS )


add_library( the_lua ${LUA_SRCS} )

add_executable(lua_adapter_test_exe 
	LuaAdapter/Test/main.cpp 
	LuaAdapter/Test/test_center.cpp
	LuaAdapter/Test/simple_test.cpp
	LuaAdapter/Test/extends_test.cpp
	LuaAdapter/Test/construction_destruction.cpp
	LuaAdapter/Classes/adapter_lua.cpp
	LuaAdapter/Classes/Class.cpp
	LuaAdapter/Classes/LuaAdapterEnvironment.cpp
	LuaAdapter/Classes/Namespace.cpp
	
)

target_link_libraries( lua_adapter_test_exe the_lua )


