#pragma once

#include "lua.hpp"

#ifndef LUA_OK
#define LUA_OK (0)
#endif

size_t la_lua_table_len(lua_State* L, int idx);
