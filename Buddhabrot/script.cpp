#include "stdafx.h"

#include "../luaattractors.h"
#include "luabuddhabrot.h"

using namespace attractors;

int __cdecl main(int argc, char* argv[]) {
	return luamain<luabuddhabrot>(argc, argv, "buddhabrot.lua");
}

// The End.
