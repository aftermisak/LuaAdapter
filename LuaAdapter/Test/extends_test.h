#pragma once
#include <string>
#include <iostream>
#include <typeinfo>
namespace LuaAdapter {

namespace extends_test {

	void extends_test();

	class ClassSupperA {
	public:
		static const char* post_code;
		std::string getMyDog() const { return mydog; }
		void setMyDog(const std::string& name) { mydog = name; }
		virtual ~ClassSupperA(){}
	private:
		std::string mydog = "keven";
	};

	class ClassSupperB {
	public:
		virtual std::string getMyCat() { return "none"; }
		static void yield() {
			std::cout << "ClassSupperB yield " << std::endl;
		}

		virtual ~ClassSupperB(){}
	};

	//multiple inheritance
	class ClassSubA : public ClassSupperA, public ClassSupperB {
	public:
		void takeAShower() {
			auto& tinfo = typeid(ClassSubA);
			std::cout << tinfo.name() << "take a shower" << std::endl;
		}
	private:
		std::string aaa;
	};

	class ClassSubB : public ClassSupperB {
	public:
		virtual std::string getMyCat()override { return mycat; }
		std::string getMyGirlfriend() { return "yuiri"; }
	private:
		std::string mycat = "bob";
	};

	class ClassSubC : public ClassSubA {
	public:
		void takeABreak() {
			auto& tinfo = typeid(ClassSubC);
			std::cout << tinfo.name() << "take a break" << std::endl;
		}
	private:
		std::string ccc;
	};

	void processClassSupperA(ClassSupperA* p);
	void processClassSupperB(ClassSupperB* p);
	void processClassSubA(ClassSubA* p);
	void processClassSubB(ClassSubB* p);
} //end ns extends_test
	

}