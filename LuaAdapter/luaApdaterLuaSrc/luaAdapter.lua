print( "load luaAdapter.lua" )

luaAdapter = {
	luaAdapterToken = "lcx looking for you"
}

function luaAdapter_lua_getNamespaceTable( nameOfNamespace )
	return luaAdapter[nameOfNamespace]
end


function luaAdapter_lua_getClassTable( nameOfNamespace, className)
	local namespaceTable = luaAdapter_lua_getNamespaceTable( nameOfNamespace )
	if( not namespaceTable ) then return nil end

	local classTable = namespaceTable.__classTable__[className]
	return classTable
	
end

--python $(ProjectDir)../../Tools/dir_cpy.py "$(ProjectDir)../../LuaAdapter" "$(ProjectDir)/LuaAdapter"