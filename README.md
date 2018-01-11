# LuaAdapter
    一个C++模板库，提供一套将命名空间、类、变量、函数等，注册到lua环境的方案。 并且支持扩展。
    它非常像另外一个开源库LuaBridge，该库使用了一些c++11的特性，对实现和支持做出了一些改动。

    使用MIT Licensed
    除了lua环境，不需要依赖别的库，但是需要编译器尽量好的支持c++11
    支持将多继承模型的cpp类注册到lua环境中

    在Test目录下，有一些测试，也是使用的例子
    simple_test 怎么注册命名空间，类，静态方法，成员方法，静态变量，成员变量到lua环境中
    extends_test 怎么注册类之间的继承关系
    construction_destruction 为类注册构造和析构，提供了一种策略来进行生命周期管理

    关于生命周期管理：
    对象的生命周期管理，是一个便随着cpp开发的问题，如果你使用类似于cocos2d-x这类的引擎，那么
栈对象的创建释放可以自动使用其提供的 create retian release等方法来创建和管理。如果你的程序没
有类似的管理，或者是不同的管理方式，LuaAdapter允许为类绑定构造和析构函数，并且为所有对象提供
了一个成员方法，可以在lua层决定userdata被lua gc回收时是否调用析构函数，也就是可以在创建对象后
把生命周期交给cpp层管理
	

    扩展lua传入cpp时支持的类型 template<> LuaParm:
    LuaParm支持了一些常见的数字，字符串，STL标准库类型，但是一旦使用，可能需要扩展，比如我的另外
一个程序中，使用了glm库，需要向cpp传glm::vec3,glm::vec4等参数，那么就需要在LuaAdapter命名空间下
实现以下
` ``cpp
	template<> struct LuaParm<glm::vec3> {
		glm::vec3 value;
		LuaParm(lua_State* L, int index) {
			lua_pushstring(L, "x");//vec3, "x"
			lua_rawget(L, -2);//vec3 x
			value.x = (float)lua_tonumber(L, -1);//vec3 x
			lua_pop(L, 1);//vec3

			lua_pushstring(L, "y");//vec3, "y"
			lua_rawget(L, -2);//vec3 y
			value.y = (float)lua_tonumber(L, -1);//vec3 y
			lua_pop(L, 1);//vec3

			lua_pushstring(L, "z");//vec3, "z"
			lua_rawget(L, -2);//vec3 z
			value.z = (float)lua_tonumber(L, -1);//vec3 z
			lua_pop(L, 1);//vec3
		}
	};
	template<> struct LuaParm<const glm::vec3&> : public LuaParm<glm::vec3> {
		using LuaParm<glm::vec3>::LuaParm;
	};
	template<> struct LuaParm<glm::vec4> {
		glm::vec4 value;
		LuaParm(lua_State* L, int index) {
			...
		}
	};
	template<> struct LuaParm<const glm::vec4&> : public LuaParm<glm::vec4> {
		using LuaParm<glm::vec4>::LuaParm;
	};
` ``

    扩展cpp传入lua时支持的类型 template<> PushToLua:
    与LuaParm类似，它是个反过程，它需要的是将一个对象正确创建并且压倒lua栈上。
` ``cpp
    template< >
	struct PushToLua<glm::vec3>{
		static void push(lua_State* L, const glm::vec3& value) {
			lua_newtable(L);
			lua_pushstring(L, "x");
			lua_pushnumber(L, value.x);
			lua_rawset(L, -3);
			lua_pushstring(L, "y");
			lua_pushnumber(L, value.y);
			lua_rawset(L, -3);
			lua_pushstring(L, "z");
			lua_pushnumber(L, value.z);
			lua_rawset(L, -3);
		}
	};

	template< > struct PushToLua<const glm::vec3&> : public PushToLua<glm::vec3> {};

    template< >
	struct PushToLua<glm::vec4> {
		static void push(lua_State* L, const glm::vec4& value) {
			lua_newtable(L);
			lua_pushstring(L, "x");
			lua_pushnumber(L, value.x);
			lua_rawset(L, -3);

			...
		}
	};

	template< > struct PushToLua<const glm::vec4&> : public PushToLua<glm::vec4> {};
` ``
