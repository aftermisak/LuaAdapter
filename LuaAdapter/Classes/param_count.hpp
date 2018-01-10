#pragma once

namespace LuaAdapter {
	template <typename Fun>
	struct ParmCount;

	/*
	note:
	compiler cant distinguish void(T::*)(...TParms) from Return(T::*)(...TParms)
	*/

	template<typename Return, typename ...TParms>//static
	struct ParmCount<Return(*)(TParms...)> {
		static const int result = sizeof...(TParms);
	};

	template<typename Return, typename T, typename ...TParms>//member
	struct ParmCount<Return(T::*)(TParms...)> {
		static const int result = sizeof...(TParms);
	};

	template<typename Return, typename T, typename ...TParms>//const member
	struct ParmCount<Return(T::*)(TParms...)const> {
		static const int result = sizeof...(TParms);
	};

}