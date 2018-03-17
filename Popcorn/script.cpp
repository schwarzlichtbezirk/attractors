#include "stdafx.h"

#include "../luaattractors.h"
#include "luapopcorn.h"

using namespace attractors;

int __cdecl main(int argc, char* argv[]) {
	return luamain<luapopcorn>(argc, argv, "popcorn.lua");
}

// The End.
