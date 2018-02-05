#pragma once
#ifndef __luastrut_h__
#define __luastrut_h__

//-----------------------------------------------------------------------------

inline std::string luaL_tombstr(lua_State* L, int numArg) {
	size_t len;
	const char* str = lua_tolstring(L, numArg, &len);
	return std::string(str, len);
}

inline std::wstring luaL_towstr(lua_State* L, int numArg) {
	size_t len;
	const char* str = lua_tolstring(L, numArg, &len);
	return utf8_to_wchar(str, len);
}

inline std::string luaL_checkmbstr(lua_State* L, int numArg) {
	size_t len;
	const char* str = luaL_checklstring(L, numArg, &len);
	return std::string(str, len);
}

inline std::wstring luaL_checkwstr(lua_State* L, int numArg) {
	size_t len;
	const char* str = luaL_checklstring(L, numArg, &len);
	return utf8_to_wchar(str, len);
}

inline std::string luaL_optmbstr(lua_State* L, int numArg, const char* def) {
	size_t len;
	const char* str = luaL_optlstring(L, numArg, def, &len);
	return std::string(str, len);
}

inline std::wstring luaL_optwstr(lua_State* L, int numArg, const char* def) {
	size_t len;
	const char* str = luaL_optlstring(L, numArg, def, &len);
	return utf8_to_wchar(str, len);
}

inline void luaL_pushwstring(lua_State* L, const std::wstring& wstr) {
	std::string str = wchar_to_utf8(wstr);
	lua_pushlstring(L, str.c_str(), str.size());
}

//-----------------------------------------------------------------------------

#endif // __luastrut_h__