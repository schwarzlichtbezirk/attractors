#pragma once

#include "../Luna.h"
#include "buddhabrot.h"

namespace attractors {

	class luabuddhabrot : public buddhabrot {
	public:

		LUNAOBJ_CLASS(luabuddhabrot);
		void tostring(lua_State *L) const;
		bool operator == (const buddhabrot& cmp) const;

		luabuddhabrot() = default;
		luabuddhabrot(const buddhabrot& v) : buddhabrot(v) {}
		luabuddhabrot(buddhabrot&& v) noexcept : buddhabrot(std::move(v)) {}

	protected:

		LUNA_GETTER(nmax);
		LUNA_SETTER(nmax);
		LUNA_GETTER(tmax);
		LUNA_SETTER(tmax);
		LUNA_GETTER(frames);
		LUNA_DECL(render);

	};

}

// The End.
