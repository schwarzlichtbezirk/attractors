
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

	std::atomic_int percent = -pool; // skip calculation on each thread start
	std::atomic_int busynum = 0;
	std::mutex mtxcout;
	std::vector<std::thread> job(pool);
	std::mutex mtxpool;
	std::condition_variable cv;

	auto pc1 = std::chrono::high_resolution_clock::now();
	for (int quote = 0; quote < pool; quote++) {
		busynum++;
		job[quote] = std::thread([&, quote]() {
			render(quote, pool, *img, color::rainbow, [&]() {
				auto pct = std::chrono::high_resolution_clock::now();
				auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(pct - pc1).count() / 1e9;
				percent++;
				mtxcout.lock(); // exclusive access to std::wcout
				std::wcout << L"\r" << percent << L"%, remains " << (percent > 0 ? (int)(dur * (100 - percent) / percent) : 0) << L"s   ";
				mtxcout.unlock();
			});
			busynum--;
			cv.notify_one();
		});
	}
	std::unique_lock<std::mutex> lck(mtxpool);
	cv.wait(lck, [&]()->bool { return busynum == 0; });
	for (auto& th : job) {
		th.join();
	}
	auto pc2 = std::chrono::high_resolution_clock::now();

	lua_pushnumber(L, std::chrono::duration_cast<std::chrono::nanoseconds>(pc2 - pc1).count() / 1e9);
	lua_pushinteger(L, nsum);
	lua_pushinteger(L, numdiscard);
	return 3;
}

//-----------------------------------------------------------------------------

// The End.
