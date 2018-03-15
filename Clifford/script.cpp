/*
	xn+1 = sin(a yn) + c cos(a xn)
	yn+1 = sin(b xn) + d cos(b yn)
*/

#include "stdafx.h"

#include "../luaattractors.h"
#include "luaclifford.h"

using namespace attractors;

static void createargtable(lua_State *L, int argc, char* argv[]) {
	if (argv) {
		lua_createtable(L, argc - 1, 1);
		for (int i = 0; i < argc; i++) {
			lua_pushstring(L, argv[i]);
			lua_rawseti(L, -2, i);
		}
		lua_setglobal(L, "cmdline");
	}
}

int __cdecl main(int argc, char* argv[]) {
	int retval = EXIT_SUCCESS;
	// Inits Lua virtual machine
	auto L = luaL_newstate();
	_ASSERT(L);
	luaL_openlibs(L);

	// insert "DevMode" indicator
	lua_pushboolean(L,
#ifdef _DEBUG
		true);
#else
		false);
#endif
	lua_setglobal(L, "devmode");
	// insert "Platform" string
	lua_pushstring(L,
#if defined(_M_IX86)
		"x86");
#elif defined(_M_X64)
		"x64");
#elif defined(_M_ARM)
		"arm");
#elif defined(_M_ARM64)
		"arm64");
#elif defined(_M_IA64)
		"ia64");
#else
		"*");
#endif
	lua_setglobal(L, "platform");
	// insert "PtrSize" value
	lua_pushinteger(L, sizeof(size_t));
	lua_setglobal(L, "ptrsize");

	createargtable(L, argc, argv);

	// register Lua data
	luacolor::Luna::Register(L);
	luaimage::Luna::Register(L);
	luaclifford::Luna::Register(L);

	// insert itself to script
	if (luaL_dofile(L, argc > 1 ? argv[1] : "clifford.lua")) {
		if (lua_isstring(L, -1)) {
			std::cout << lua_tostring(L, -1) << std::endl;
		}
		retval = EXIT_FAILURE;
	}
	else {
		_ASSERT(lua_gettop(L) == 0); // Lua stack must be empty
									 // call Lua event
		lua_getglobal(L, "main");
		if (lua_isfunction(L, -1)) {
			if (lua_pcall(L, 0, 0, 0)) {
				std::cout << lua_tostring(L, -1) << std::endl;
				retval = EXIT_FAILURE;
			}
		}
		else lua_pop(L, 1);
	}

	// Close Lua virtual machine
	lua_close(L);
	L = nullptr;

	return retval;
}

// The End.
