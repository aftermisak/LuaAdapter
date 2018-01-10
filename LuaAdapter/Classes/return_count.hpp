#pragma once

namespace LuaAdapter {
	template <typename Fun>
	struct ReturnCount;

	/*
	note:
	compiler cant distinguish void(T::*)(...TParms) from Return(T::*)(...TParms)
	*/

	template<typename Return, typename ...TParms>//static
	struct ReturnCount<Return(*)(TParms...)> {
		static const int result = std::is_void<Return>::value ? 0 : 1;
	};

	template<typename Return, typename T, typename ...TParms>//member
	struct ReturnCount<Return(T::*)(TParms...)> {
		static const int result = std::is_void<Return>::value ? 0 : 1;
	};

	template<typename Return, typename T, typename ...TParms>//const member 
	struct ReturnCount<Return(T::*)(TParms...)const> {
		static const int result = std::is_void<Return>::value ? 0 : 1;
	};

}