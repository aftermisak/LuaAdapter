#pragma once
#include <iostream>
#include <string>
namespace LuaAdapter{
	namespace simple_test {

		void simple_test();
		class ClassA {
		public:
			int memberFunction1(char a, unsigned char b, int c, unsigned d) {
				std::cout << "memberFunction1" << a << "  " << b << std::endl;
				std::cout << "memberFunction1" << c << "  " << d << std::endl;
				return 0;
			}

			unsigned memberFunction2(short a, unsigned short b, long long c, unsigned long long d) {
				std::cout << "memberFunction2" << a << "  " << b << std::endl;
				std::cout << "memberFunction2" << c << "  " << d << std::endl;
				return 10u;
			}

			std::string memberFunction3(std::string a, const std::string& b, const std::string c, const char* d) {
				std::cout << "memberFunction3" << a << b << c << d << std::endl;
				return "";
			}
			static void staticFunction1() {
				std::cout << "staticFunction1" << std::endl;
			}
			static const char * staticFunction2() {
				std::cout << "staticFunction1" << std::endl;
				return "aaaaaaaaaaaaaaaaaaaccccccccccccccccccccccccccccccc";
			}

			int memberVariable1 = 0;
			const std::string memberVariable2 = "ccccccccc";
			static unsigned staticVariable1;
			static const int staticVariable2;
		private:

		};
	}//end ns simple_test
}