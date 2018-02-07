
#include "stdafx.h"

#include "../luaattractors.h"
#include "luabuddhabrot.h"
#include "../stringutil.h"
#include "../luastrut.h"

using namespace attractors;

//-----------------------------------------------------------------------------

template luabuddhabrot::Luna;
const char luabuddhabrot::classname[] = "buddhabrot";
const luabuddhabrot::PropertyType luabuddhabrot::properties[] = {
	LUNA_PROP(nmax),
	LUNA_PROP(tmax),
	LUNA_RPROP(frames),
	LUNA_METHOD(render),
	{ nullptr }
};
const luabuddhabrot::FunctionType luabuddhabrot::methods[] = {
	{ nullptr }
};

void luabuddhabrot::tostring(lua_State *L) const {
	lua_pushfstring(L, "(%d,%d)", nmax, tmax);
}

bool luabuddhabrot::operator == (const buddhabrot& cmp) const {
	return nmax == cmp.nmax && tmax == cmp.tmax;
}

LUNA_GETTEREX(luabuddhabrot, nmax) {
	lua_pushinteger(L, nmax);
	return 1;
}

LUNA_SETTEREX(luabuddhabrot, nmax) {
	nmax = (int)luaL_checkinteger(L, 1);
	return 0;
}

LUNA_GETTEREX(luabuddhabrot, tmax) {
	lua_pushinteger(L, tmax);
	return 1;
}

LUNA_SETTEREX(luabuddhabrot, tmax) {
	tmax = (int)luaL_checkinteger(L, 1);
	return 0;
}

LUNA_GETTEREX(luabuddhabrot, frames) {
	lua_pushinteger(L, 1000000 * tmax);
	return 1;
}

LUNA_DECLEX(luabuddhabrot, render) {
	int pool = (int)luaL_checkinteger(L, 1);
	auto img = luaimage::Luna::check(L, 2);
	int opt = luaL_checkoption(L, 3, "rainbow", luacolor::colfltopt);

	number dur = rendermt(pool, *img, luacolor::colflttbl[opt], true);

	lua_pushnumber(L, dur);
	lua_pushinteger(L, nsum);
	lua_pushinteger(L, numdiscard);
	return 3;
}

//-----------------------------------------------------------------------------

// The End.
