
#include "test_center.h"
#include "construction_destruction.h"
#include "simple_test.h"
#include "extends_test.h"

namespace LuaAdapter {

	

	void luaadapter_test() {
		
		simple_test::simple_test();
	
		extends_test::extends_test();

		cons_des_test::test();
		
	}

}