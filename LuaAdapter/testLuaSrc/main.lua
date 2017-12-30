

local succ, err = xpcall( function() 
	local cpp = luaAdapter["cpp"]


	print( cpp.staticF1(2, 2.3) )

	print( cpp.staticV1 )

	cpp.staticV1 = 2

	print( cpp.staticV1 )

	print( cpp.A )

	print( cpp.A.staticF3 )

	print( cpp.A.staticF3( 2 ) )

	print( cpp.A.staticV2 )

	cpp.A.staticV2 = 3

	print( cpp.A.staticV2 )

	-- local aInstance1 = cpp.A.newInstance( "construct1",  "parm ")
	local aInstance1 = cpp.A.create( "param" )

	print( aInstance1 )

	print( aInstance1.memberF2)

	print( aInstance1:memberF2( 2, 3 ) )

	print( aInstance1.memberV3)

	aInstance1.memberV3 = 100

	print( aInstance1.memberV3 )

	-- local bInstance1 = cpp.B.newInstance("construct1", "35235");
	local bInstance1 = cpp.B.create( "35235" )

	print( bInstance1.memberVB1 )
	bInstance1:memberBF1()
	bInstance1:memberF2( 3, 4 ) -- TO DO fix 获取不到父类的member
	print( bInstance1.memberV3 )
	bInstance1.memberV3 = 1234325
	print( bInstance1.memberV3 )

end, function( oerr ) 
	return debug.traceback( oerr, 2 )
end )

if not succ then 
	print( err )
end