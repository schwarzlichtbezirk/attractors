
#include "stdafx.h"

#include "../luaattractors.h"
#include "luapopcorn.h"
#include "../stringutil.h"
#include "../luastrut.h"

using namespace attractors;

//-----------------------------------------------------------------------------

template luapopcorn::Luna;
const char luapopcorn::classname[] = "popcorn";
const luapopcorn::PropertyType luapopcorn::properties[] = {
	LUNA_PROP(frames),
	LUNA_PROP(iters),
	LUNA_PROP(minx),
	LUNA_PROP(maxx),
	LUNA_PROP(miny),
	LUNA_PROP(maxy),
	LUNA_PROP(h),
	LUNA_PROP(k),
	LUNA_METHOD(render),
	{ nullptr }
};
const luapopcorn::FunctionType luapopcorn::methods[] = {
	{ nullptr }
};

void luapopcorn::tostring(lua_State *L) const {
	lua_pushfstring(L, "((%d,%d)x[%f,%f]y[%f,%f]h[%f]k[%f])", frames, iters,
		minX, maxX,
		minY, maxY,
		h, k);
}

bool luapopcorn::operator == (const popcorn& cmp) const {
	return frames == cmp.frames && iters == cmp.iters &&
		minX == cmp.minX && maxX == cmp.maxX &&
		minY == cmp.minY && maxY == cmp.maxY &&
		h == cmp.h && k == cmp.k;
}

LUNA_GETTEREX(luapopcorn, frames) {
	lua_pushinteger(L, frames);
	return 1;
}

LUNA_SETTEREX(luapopcorn, frames) {
	frames = (int)luaL_checkinteger(L, 1);
	return 0;
}

LUNA_GETTEREX(luapopcorn, iters) {
	lua_pushinteger(L, iters);
	return 1;
}

LUNA_SETTEREX(luapopcorn, iters) {
	iters = (int)luaL_checkinteger(L, 1);
	return 0;
}

LUNA_GETTEREX(luapopcorn, minx) {
	lua_pushnumber(L, minX);
	return 1;
}

LUNA_SETTEREX(luapopcorn, minx) {
	minX = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luapopcorn, maxx) {
	lua_pushnumber(L, maxX);
	return 1;
}

LUNA_SETTEREX(luapopcorn, maxx) {
	maxX = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luapopcorn, miny) {
	lua_pushnumber(L, minY);
	return 1;
}

LUNA_SETTEREX(luapopcorn, miny) {
	minY = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luapopcorn, maxy) {
	lua_pushnumber(L, maxY);
	return 1;
}

LUNA_SETTEREX(luapopcorn, maxy) {
	maxY = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luapopcorn, h) {
	lua_pushnumber(L, h);
	return 1;
}

LUNA_SETTEREX(luapopcorn, h) {
	h = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luapopcorn, k) {
	lua_pushnumber(L, k);
	return 1;
}

LUNA_SETTEREX(luapopcorn, k) {
	k = luaL_checknumber(L, 1);
	return 0;
}

LUNA_DECLEX(luapopcorn, render) {
	int pool = (int)luaL_checkinteger(L, 1);
	auto img = luaimage::Luna::check(L, 2);
	int opt = luaL_checkoption(L, 3, "gradient", luacolor::colfltopt);

	number dur = rendermt(pool, *img, luacolor::colflttbl[opt], true);

	lua_pushnumber(L, dur);
	return 1;
}

//-----------------------------------------------------------------------------

// The End.
