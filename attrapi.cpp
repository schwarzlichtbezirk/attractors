
#include "stdafx.h"

#include "attrapi.h"
#include "stringutil.h"
#include "luastrut.h"

using namespace attractors;

//-----------------------------------------------------------------------------

template luaimage::Luna;
const char luaimage::classname[] = "image";
const luaimage::PropertyType luaimage::properties[] = {
	LUNA_PROP(nx),
	LUNA_PROP(ny),
	LUNA_METHOD(resize),
	LUNA_METHOD(clear),
	LUNA_METHOD(writetga),
	{ nullptr }
};
const luaimage::FunctionType luaimage::methods[] = {
	LUNA_FUNC(__len),
	LUNA_FUNC(create),
	{ nullptr }
};

void luaimage::tostring(lua_State *L) const {
	lua_pushfstring(L, "[%dx%d]", nx, ny);
}

bool luaimage::operator == (const image& cmp) const {
	return nx == cmp.nx && ny == cmp.ny;
}

LUNA_DECLEX(luaimage, __len) {
	auto op = luaimage::Luna::check(L, 1);

	lua_pushinteger(L, op->nx * op->ny);
	return 1;
}

LUNA_DECLEX(luaimage, create) {
	int nx = (int)luaL_checkinteger(L, 1);
	int ny = (int)luaL_checkinteger(L, 2);
	luaimage::Luna::push(L, luaimage(nx, ny));
	return 1;
}

LUNA_GETTEREX(luaimage, nx) {
	lua_pushinteger(L, nx);
	return 1;
}

LUNA_SETTEREX(luaimage, nx) {
	nx = (int)luaL_checkinteger(L, 1);
	update();
	return 0;
}

LUNA_GETTEREX(luaimage, ny) {
	lua_pushinteger(L, ny);
	return 1;
}

LUNA_SETTEREX(luaimage, ny) {
	ny = (int)luaL_checkinteger(L, 1);
	update();
	return 0;
}

LUNA_DECLEX(luaimage, resize) {
	nx = (int)luaL_checkinteger(L, 1);
	ny = (int)luaL_checkinteger(L, 2);
	update();
	return 0;
}

LUNA_DECLEX(luaimage, clear) {
	clear();
	return 0;
}

LUNA_DECLEX(luaimage, writetga) {
	auto filename = luaL_checkwstr(L, 1);
	auto sensitivity = (double)luaL_optnumber(L, 2, 0.02);
	writetga(filename.c_str(), sensitivity);
	return 0;
}

//-----------------------------------------------------------------------------

// The End.
