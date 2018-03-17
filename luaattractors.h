#pragma once

#include "Luna.h"
#include "attractors.h"

namespace attractors {

	class luacolor : public color {
	public:

		LUNAOBJ_CLASS(luacolor);
		void tostring(lua_State *L) const;
		bool operator == (const color& cmp) const;

		luacolor() = default;
		luacolor(const color& v) : color(v) {}
		luacolor(color&& v) noexcept : color(std::move(v)) {}
		luacolor(number r, number g, number b);

		static color monochrome(number h);
		static color gradient(number h);

		static const char* colfltopt[];
		static filter colflttbl[];

	protected:

		static LUNA_DECL(__add);
		static LUNA_DECL(create);
		static LUNA_DECL(setpair);
		static LUNA_DECL(hue);
		static LUNA_DECL(rainbow);
		static LUNA_DECL(monochrome);
		static LUNA_DECL(gradient);

		LUNA_GETTER(red);
		LUNA_SETTER(red);
		LUNA_GETTER(green);
		LUNA_SETTER(green);
		LUNA_GETTER(blue);
		LUNA_SETTER(blue);
		LUNA_DECL(normalize);
	};

	class luaimage : public image {
	public:

		LUNAOBJ_CLASS(luaimage);
		void tostring(lua_State *L) const;
		bool operator == (const image& cmp) const;

		luaimage() = default;
		luaimage(int w, int h) : image(w, h) {}

	protected:

		static LUNA_DECL(__len);
		static LUNA_DECL(create);

		LUNA_GETTER(nx);
		LUNA_SETTER(nx);
		LUNA_GETTER(ny);
		LUNA_SETTER(ny);
		LUNA_DECL(resize);
		LUNA_DECL(clear);
		LUNA_DECL(writetga);
	};

	lua_State* open_lvm();
	void createargtable(lua_State *L, int argc, char* argv[]);
	int doscript(lua_State *L, const char* script);

	template<class geometry>
	int luamain(int argc, char* argv[], const char* script) {
		// Open Lua virtual machine
		auto L = open_lvm();
		createargtable(L, argc, argv);

		// Register Lua data
		luacolor::Luna::Register(L);
		luaimage::Luna::Register(L);
		geometry::Luna::Register(L);

		// Perform the job
		int retval = doscript(L, argc > 1 ? argv[1] : script);

		// Close Lua virtual machine
		lua_close(L);
		L = nullptr;

		return retval;
	}

}

// The End.
