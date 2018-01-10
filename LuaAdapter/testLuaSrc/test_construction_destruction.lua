


local succ, err = xpcall( function() 
	local cons_des_test = luaAdapter["cons_des_test"]
	local classA = cons_des_test["an_classA"]

	local function runA1()
		local a1 = classA.newInstance( "newA1" )
		a1:goHome()
	end
	runA1()
	collectgarbage() --cause a1 des
	local a2 = classA.newInstance( "newA2", 100 ) 
	a2:goSchool()

	local a3 = classA.createA()-- this do not cause destructor, because it's not a constructor

	local a4 = classA.newInstance( "newA2", 30 ) 
	a4:no_destruction__() --this close destructor, when you want to create obj at lua, and pass to cpp and do not use it, you can do this

	print( "-----------------------------------------------------------" )
end, function( oerr ) 
	return debug.traceback( oerr, 2 )
end )

collectgarbage() --cause all destruction inneed
if not succ then 
	print( err )
end 