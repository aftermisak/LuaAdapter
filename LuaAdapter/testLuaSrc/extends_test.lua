


local succ, err = xpcall( function() 
	print( string.rep( "extends_test\n", 3 ) )

	local extends_test = luaAdapter["extends_test"]
	local ClassSupperA = extends_test["ext_ClassSupperA"]
	local ClassSupperB = extends_test["ext_ClassSupperB"]
	local ClassSubA = extends_test["ext_ClassSubA"]
	local ClassSubB = extends_test["ext_ClassSubB"]
	local ClassSubC = extends_test["ext_ClassSubC"]
	print( "aaaaaaaa", ClassSupperB.simpleCreate():getMyCat() )

	local objSubA = ClassSubA.simpleCreate()
	print( "objSubA:getMyDog()", objSubA:getMyDog() )
	print( "objSubA:setMyDog()", objSubA:setMyDog( "bob" ), objSubA:getMyDog())
	print( "objSubA:getMyCat", objSubA.getMyCat )
	print( "objSubA:getMyCat()", objSubA:getMyCat( ) )
	print( "objSubA:takeAShower()", objSubA:takeAShower( ) )


	local objSubB = ClassSubB.simpleCreate()
	print( "objSubB:getMyCat()", objSubB:getMyCat() )
	print( "objSubB:getMyGirlfriend()", objSubB:getMyGirlfriend() )

	local objSubC = ClassSubC.simpleCreate()
	print( "objSubC:getMyCat()", objSubC:takeABreak() )
	print( "objSubC:getMyDog()", objSubC:getMyDog() )
	print( "objSubC:setMyDog()", objSubC:setMyDog( "bob" ), objSubC:getMyDog())
	print( "objSubC:getMyCat", objSubC.getMyCat )
	print( "objSubC:getMyCat()", objSubC:getMyCat( ) )
	print( "objSubC:takeAShower()", objSubC:takeAShower( ) )


	print( "-------------------------------------------------------------" )
	print( "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" )
	extends_test.processClassSupperA( ClassSupperA.simpleCreate() )
	extends_test.processClassSupperA( ClassSubA.simpleCreate() )
	extends_test.processClassSupperA( ClassSubC.simpleCreate() )

	extends_test.processClassSupperB( ClassSupperB.simpleCreate() )
	extends_test.processClassSupperB( ClassSubA.simpleCreate() )
	extends_test.processClassSupperB( ClassSubB.simpleCreate() )
	extends_test.processClassSupperB( ClassSubC.simpleCreate() )

	extends_test.processClassSubA( ClassSubA.simpleCreate() )
	extends_test.processClassSubA( ClassSubC.simpleCreate() )
	
	extends_test.processClassSubB( ClassSubB.simpleCreate() )

end, function( oerr ) 
	return debug.traceback( oerr, 2 )
end )


if not succ then 
	print( err )
end 