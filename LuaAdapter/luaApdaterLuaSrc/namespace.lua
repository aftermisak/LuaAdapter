print( "load namespace.lua" )

function luaAdapter_lua_createNamespace()
	local namespaceTable = {}

	namespaceTable.luaAdapterNamespaceToken = "hidden in dark"
	namespaceTable.__classTable__ = {}
	namespaceTable.__staticFuncTable__ = {}
	namespaceTable.__dataGetTable__ = {}
	namespaceTable.__dataSetTable__ = {}

	local mt = {}
	mt.__index = function(table, name )

		local func = table.__staticFuncTable__[name]
		if func then return func end

		local dataGet = table.__dataGetTable__[name]
		if dataGet then return dataGet() end

		local class = table.__classTable__[name]
		if class then return class end
		
		error("namespace doesn't have the field : " .. name);
	end

	mt.__newindex = function(table, name, value)
		local dataSet = table.__dataSetTable__[name]
		if dataSet then return dataSet( value ) end

		error("namespace doesn't have the data : " .. name) 
	end

	setmetatable(namespaceTable, mt)
	return namespaceTable;
end




