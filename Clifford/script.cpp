﻿#include "stdafx.h"

#include "../luaattractors.h"
#include "luaclifford.h"

using namespace attractors;

int __cdecl main(int argc, char* argv[]) {
	return luamain<luaclifford>(argc, argv, "clifford.lua");
}

// The End.
