--[[

	Rosetta https://www.rosettacommons.org/manuals/archive/rosetta_2016.31.58825_user_guide/all_else/index.html
	clang  https://clang.llvm.org/doxygen/index.html
]]

print( "load class.lua" )

local __all_classes__ = {
	
}

local __all_classes_obj_metatable__ = {
	
}


local createObjectMetatable --pre define

function luaAdapter_lua_createClassTable(className)
	assert( type(className) == "string" )
	if __all_classes__[className] then 
		error( string.format("already have class with name:%s", className) )
	end 

	local classTable = {}
	__all_classes__[className] = classTable

	classTable.__className__ = className

	classTable.__supperClasses__ = {}

	classTable.__Constructors__ = {}

	classTable.__Destructor__ = function( ... )
		-- error("not implement destructor")
		-- print( "default destructor do nothing" )
	end

	classTable.__staticFuncTable__ = {}

	classTable.__DataGetTable__ = {}

	classTable.__DataSetTable__ = {}



	classTable.__FuncMemberTable__ = {}
	
	classTable.__DataMemberGetTable__ = {}

	classTable.__DataMemberSetTable__ = {}

	


	local mt = {}

	mt.__index = function(table, name)
		local staticFunc = table.__staticFuncTable__[name]
		if(staticFunc) then return staticFunc end

		local dataGet = table.__DataGetTable__[name]
		if(dataGet) then return dataGet() end

		error("do not have static field :" .. name)
	end

	mt.__newindex = function (table, name, value)
		local dataSet = table.__DataSetTable__[name]
		if dataSet then return dataSet( value ) end

		error("do not have static data :" .. name)
	end

	classTable.newInstance = function( funcName, ...)
		assert( funcName and type(funcName) == "string" )
		local func = classTable.__Constructors__[funcName]

		if( func ) then
			local instance = func(...)-- userdata's metatable set by c code
			return instance
		end

		error( "do not have constrcutor " .. funcName )
		return nil
	end

	setmetatable(classTable, mt)
	createObjectMetatable( className )
	return classTable
end

function luaAdapter_lua_getClassTable( className )
	return __all_classes__[className]
end 

local function findMember( aClassTable, memberTableName, fieldName )
	-- print ( "findMember", aClassTable.__className__, memberTableName, fieldName )
	local member = aClassTable[memberTableName][fieldName]
	if member ~= nil then return member end

	for i, aSupperClassTable in ipairs( aClassTable.__supperClasses__ ) do
		member = findMember(aSupperClassTable, memberTableName, fieldName)
		if member ~= nil then return member end
	end

	return nil
end

local function findFunctionMember(aClassTable, functionName)
	-- print( "findFunctionMember:", aClassTable.__className__, functionName )
	return findMember(aClassTable, "__FuncMemberTable__", functionName)
end

local function findDataMemberGet(aClassTable, dataName)
	-- print( "findDataMemberGet:", aClassTable.__className__, dataName )
	return findMember(aClassTable, "__DataMemberGetTable__", dataName)
end

local function findDataMemberSet(aClassTable, dataName)
	-- print( "findDataMemberSet:", aClassTable.__className__, dataName )
	return findMember(aClassTable, "__DataMemberSetTable__", dataName)
end


createObjectMetatable = function( className )
	assert( type(className) == "string" )

	local classTable = __all_classes__[className]
	if not classTable then error( string.format( "no class with name:%s", className ) ) end 

	if __all_classes_obj_metatable__[className] then
		error( string.format("already have obj metatable for class:%s", className) )
	end 
	local mt = {}
	__all_classes_obj_metatable__[className] = mt

	mt.__index = function(userObj, name)
		--[[
			这里先尝试获取function member, 因为LuaAdapter并不提倡过多向lua中注册C++变量，而更提倡注册C++函数。
		通过函数来封装你的逻辑,再让lua来调用,而不是通过lua来做这些直接操作数据的逻辑。
		]]
		local funcMember = findFunctionMember(classTable, name);
		if( funcMember ) then return funcMember end

		local dataMemberGet = findDataMemberGet(classTable, name);
		if(dataMemberGet) then return dataMemberGet( userObj ) end

		-- sp
		if name == "no_destruction__" then 
			return assert( classTable.__staticFuncTable__[name] )
		end 

		error( "object has no member field:" .. name )
	end

	mt.__newindex = function( userObj, name, value )
		local  dataMemberSet = findDataMemberSet( classTable, name );
		if( dataMemberSet ) then return dataMemberSet( userObj, value ) end

		error( "object has no member field:" .. name )
	end

	mt.__gc = function( userObj )
		local destructor = classTable.__Destructor__
		if destructor then 
			destructor( userObj )
		end 
	end
	
	return mt
end

function luaAdapter_lua_getObjectMetatable( className )
	return __all_classes_obj_metatable__[className]
end 
