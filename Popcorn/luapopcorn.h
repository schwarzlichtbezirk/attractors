#pragma once

#include "../Luna.h"
#include "popcorn.h"

namespace attractors {

	class luapopcorn : public popcorn {
	public:

		LUNAOBJ_CLASS(luapopcorn);
		void tostring(lua_State *L) const;
		bool operator == (const popcorn& cmp) const;

		luapopcorn() = default;
		luapopcorn(const popcorn& v) : popcorn(v) {}
		luapopcorn(popcorn&& v) noexcept : popcorn(std::move(v)) {}

	protected:

		LUNA_GETTER(frames);
		LUNA_SETTER(frames);
		LUNA_GETTER(iters);
		LUNA_SETTER(iters);
		LUNA_GETTER(minx);
		LUNA_SETTER(minx);
		LUNA_GETTER(maxx);
		LUNA_SETTER(maxx);
		LUNA_GETTER(miny);
		LUNA_SETTER(miny);
		LUNA_GETTER(maxy);
		LUNA_SETTER(maxy);
		LUNA_GETTER(h);
		LUNA_SETTER(h);
		LUNA_GETTER(k);
		LUNA_SETTER(k);
		LUNA_DECL(render);

	};

}

// The End.
