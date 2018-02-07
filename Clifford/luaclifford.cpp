
#include "stdafx.h"

#include "../luaattractors.h"
#include "luaclifford.h"
#include "../stringutil.h"
#include "../luastrut.h"

using namespace attractors;

//-----------------------------------------------------------------------------

template luaclifford::Luna;
const char luaclifford::classname[] = "clifford";
const luaclifford::PropertyType luaclifford::properties[] = {
	LUNA_PROP(frames),
	LUNA_PROP(iters),
	LUNA_PROP(skip),
	LUNA_PROP(minx),
	LUNA_PROP(maxx),
	LUNA_PROP(miny),
	LUNA_PROP(maxy),
	LUNA_PROP(mina),
	LUNA_PROP(maxa),
	LUNA_PROP(minb),
	LUNA_PROP(maxb),
	LUNA_PROP(minc),
	LUNA_PROP(maxc),
	LUNA_PROP(mind),
	LUNA_PROP(maxd),
	LUNA_METHOD(render),
	{ nullptr }
};
const luaclifford::FunctionType luaclifford::methods[] = {
	{ nullptr }
};

void luaclifford::tostring(lua_State *L) const {
	lua_pushfstring(L, "((%d,%d,%d)x[%f,%f]y[%f,%f]a[%f,%f]b[%f,%f]c[%f,%f]d[%f,%f])", frames, iters, skip,
		minX, maxX,
		minY, maxY,
		minA, maxA,
		minB, maxB,
		minC, maxC,
		minD, maxD);
}

bool luaclifford::operator == (const clifford& cmp) const {
	return frames == cmp.frames && iters == cmp.iters && skip == cmp.skip &&
		minX == cmp.minX && maxX == cmp.maxX &&
		minY == cmp.minY && maxY == cmp.maxY &&
		minA == cmp.minA && maxA == cmp.maxA &&
		minB == cmp.minB && maxB == cmp.maxB &&
		minC == cmp.minC && maxC == cmp.maxC &&
		minD == cmp.minD && maxD == cmp.maxD;
}

LUNA_GETTEREX(luaclifford, frames) {
	lua_pushinteger(L, frames);
	return 1;
}

LUNA_SETTEREX(luaclifford, frames) {
	frames = (int)luaL_checkinteger(L, 1);
	return 0;
}

LUNA_GETTEREX(luaclifford, iters) {
	lua_pushinteger(L, iters);
	return 1;
}

LUNA_SETTEREX(luaclifford, iters) {
	iters = (int)luaL_checkinteger(L, 1);
	return 0;
}

LUNA_GETTEREX(luaclifford, skip) {
	lua_pushinteger(L, skip);
	return 1;
}

LUNA_SETTEREX(luaclifford, skip) {
	skip = (int)luaL_checkinteger(L, 1);
	return 0;
}

LUNA_GETTEREX(luaclifford, minx) {
	lua_pushnumber(L, minX);
	return 1;
}

LUNA_SETTEREX(luaclifford, minx) {
	minX = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luaclifford, maxx) {
	lua_pushnumber(L, maxX);
	return 1;
}

LUNA_SETTEREX(luaclifford, maxx) {
	maxX = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luaclifford, miny) {
	lua_pushnumber(L, minY);
	return 1;
}

LUNA_SETTEREX(luaclifford, miny) {
	minY = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luaclifford, maxy) {
	lua_pushnumber(L, maxY);
	return 1;
}

LUNA_SETTEREX(luaclifford, maxy) {
	maxY = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luaclifford, mina) {
	lua_pushnumber(L, minA);
	return 1;
}

LUNA_SETTEREX(luaclifford, mina) {
	minA = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luaclifford, maxa) {
	lua_pushnumber(L, maxA);
	return 1;
}

LUNA_SETTEREX(luaclifford, maxa) {
	maxA = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luaclifford, minb) {
	lua_pushnumber(L, minB);
	return 1;
}

LUNA_SETTEREX(luaclifford, minb) {
	minB = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luaclifford, maxb) {
	lua_pushnumber(L, maxB);
	return 1;
}

LUNA_SETTEREX(luaclifford, maxb) {
	maxB = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luaclifford, minc) {
	lua_pushnumber(L, minC);
	return 1;
}

LUNA_SETTEREX(luaclifford, minc) {
	minC = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luaclifford, maxc) {
	lua_pushnumber(L, maxC);
	return 1;
}

LUNA_SETTEREX(luaclifford, maxc) {
	maxC = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luaclifford, mind) {
	lua_pushnumber(L, minD);
	return 1;
}

LUNA_SETTEREX(luaclifford, mind) {
	minD = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luaclifford, maxd) {
	lua_pushnumber(L, maxD);
	return 1;
}

LUNA_SETTEREX(luaclifford, maxd) {
	maxD = luaL_checknumber(L, 1);
	return 0;
}

LUNA_DECLEX(luaclifford, render) {
	int pool = (int)luaL_checkinteger(L, 1);
	auto img = luaimage::Luna::check(L, 2);
	int opt = luaL_checkoption(L, 3, "hue", luacolor::colfltopt);

	number dur = rendermt(pool, *img, luacolor::colflttbl[opt], true);

	lua_pushnumber(L, dur);
	return 1;
}

//-----------------------------------------------------------------------------

// The End.
