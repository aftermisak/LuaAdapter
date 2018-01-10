


local succ, err = xpcall( function() 
	local simple_test = luaAdapter["simple_test"]
	local ClassA = simple_test["simple_test.ClassA"]

	local a = ClassA.createA()
	print("memberVariable1", a.memberVariable1)
	a.memberVariable1 = 13
	print("memberVariable1", a.memberVariable1)

	print("memberVariable2", a.memberVariable2)
	-- a.memberVariable2 = "fu*k you" --cause read only error

	print( "memberFunction1", a:memberFunction1( string.byte("a", 1), 2, 3, 4 ) )
	print( "memberFunction2", a:memberFunction2( 1, 2, 3, 4 ) )
	print( "memberFunction3", a:memberFunction3( "a", "b", "c", "d" ) )

	print("staticVariable1", ClassA.staticVariable1)
	ClassA.staticVariable1 = 18
	print("staticVariable1", ClassA.staticVariable1)	

	print("staticVariable2", ClassA.staticVariable2)
	-- ClassA.staticVariable2 = 18 --cause read only error

	print( "staticFunction1", ClassA.staticFunction1() )
	print( "staticFunction2", ClassA.staticFunction2() )	


end, function( oerr ) 
	return debug.traceback( oerr, 2 )
end )


if not succ then 
	print( err )
end 