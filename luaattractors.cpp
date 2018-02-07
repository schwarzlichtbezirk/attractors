
#include "stdafx.h"

#include "luaattractors.h"
#include "stringutil.h"
#include "luastrut.h"

using namespace attractors;

//-----------------------------------------------------------------------------

static luacolor::color hue1(0.8, 0.25, 1.0), hue2(0.25, 1.0, 0.0);

color luacolor::monochrome(number h) {
	return color::monochrome(h, hue1);
}

color luacolor::gradient(number h) {
	return color::gradient(h, hue1, hue2);
}

const char* luacolor::colfltopt[] = {
	"hue",
	"rainbow",
	"mono",
	"gradient",
	nullptr
};

color::filter luacolor::colflttbl[] = {
	color::hue,
	color::rainbow,
	luacolor::monochrome,
	luacolor::gradient,
	nullptr
};

template luacolor::Luna;
const char luacolor::classname[] = "color";
const luacolor::PropertyType luacolor::properties[] = {
	LUNA_PROP(red),
	LUNA_PROP(green),
	LUNA_PROP(blue),
	LUNA_METHOD(normalize),
	{ nullptr }
};
const luacolor::FunctionType luacolor::methods[] = {
	LUNA_FUNC(__add),
	LUNA_FUNC(create),
	LUNA_FUNC(setpair),
	LUNA_FUNC(hue),
	LUNA_FUNC(rainbow),
	LUNA_FUNC(monochrome),
	LUNA_FUNC(gradient),
	{ nullptr }
};

void luacolor::tostring(lua_State *L) const {
	lua_pushfstring(L, "(%f,%f,%f)", red, green, blue);
}

bool luacolor::operator == (const color& cmp) const {
	return red == cmp.red && green == cmp.green && blue == cmp.blue;
}

luacolor::luacolor(number r, number g, number b) {
	red = r, green = g, blue = b;
}

LUNA_DECLEX(luacolor, __add) {
	auto op1 = luacolor::Luna::check(L, 1);
	auto op2 = luacolor::Luna::check(L, 2);

	luacolor::Luna::push(L, *op1 + *op2);
	return 1;
}

LUNA_DECLEX(luacolor, create) {
	auto r = luaL_checknumber(L, 1);
	auto g = luaL_checknumber(L, 2);
	auto b = luaL_checknumber(L, 3);

	luacolor::Luna::push(L, luacolor(r, g, b));
	return 1;
}

LUNA_DECLEX(luacolor, setpair) {
	auto op1 = luacolor::Luna::check(L, 1);
	auto op2 = luacolor::Luna::check(L, 2);

	hue1 = *op1, hue2 = *op2;
	return 0;
}

LUNA_DECLEX(luacolor, hue) {
	auto h = luaL_checknumber(L, 1);

	luacolor::Luna::push(L, hue(h));
	return 1;
}

LUNA_DECLEX(luacolor, rainbow) {
	auto h = luaL_checknumber(L, 1);

	luacolor::Luna::push(L, rainbow(h));
	return 1;
}

LUNA_DECLEX(luacolor, monochrome) {
	auto h = luaL_checknumber(L, 1);
	auto op1 = luacolor::Luna::check(L, 2);

	luacolor::Luna::push(L, color::monochrome(h, *op1));
	return 1;
}

LUNA_DECLEX(luacolor, gradient) {
	auto h = luaL_checknumber(L, 1);
	auto op1 = luacolor::Luna::check(L, 2);
	auto op2 = luacolor::Luna::check(L, 3);

	luacolor::Luna::push(L, color::gradient(h, *op1, *op2));
	return 1;
}

LUNA_GETTEREX(luacolor, red) {
	lua_pushnumber(L, red);
	return 1;
}

LUNA_SETTEREX(luacolor, red) {
	red = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luacolor, green) {
	lua_pushnumber(L, green);
	return 1;
}

LUNA_SETTEREX(luacolor, green) {
	green = luaL_checknumber(L, 1);
	return 0;
}

LUNA_GETTEREX(luacolor, blue) {
	lua_pushnumber(L, blue);
	return 1;
}

LUNA_SETTEREX(luacolor, blue) {
	blue = luaL_checknumber(L, 1);
	return 0;
}

LUNA_DECLEX(luacolor, normalize) {
	normalize();
	return 0;
}

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
	auto sensitivity = (number)luaL_optnumber(L, 2, 0.02);
	writetga(filename.c_str(), sensitivity);
	return 0;
}

//-----------------------------------------------------------------------------

// The End.
