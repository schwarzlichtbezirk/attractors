#pragma once

#include "../Luna.h"
#include "clifford.h"

namespace attractors {

	class luaclifford : public clifford {
	public:

		LUNAOBJ_CLASS(luaclifford);
		void tostring(lua_State *L) const;
		bool operator == (const clifford& cmp) const;

		luaclifford() = default;
		luaclifford(const clifford& v) : clifford(v) {}
		luaclifford(clifford&& v) noexcept : clifford(std::move(v)) {}

	protected:

		LUNA_GETTER(frames);
		LUNA_SETTER(frames);
		LUNA_GETTER(iters);
		LUNA_SETTER(iters);
		LUNA_GETTER(skip);
		LUNA_SETTER(skip);
		LUNA_GETTER(minx);
		LUNA_SETTER(minx);
		LUNA_GETTER(maxx);
		LUNA_SETTER(maxx);
		LUNA_GETTER(miny);
		LUNA_SETTER(miny);
		LUNA_GETTER(maxy);
		LUNA_SETTER(maxy);
		LUNA_GETTER(mina);
		LUNA_SETTER(mina);
		LUNA_GETTER(maxa);
		LUNA_SETTER(maxa);
		LUNA_GETTER(minb);
		LUNA_SETTER(minb);
		LUNA_GETTER(maxb);
		LUNA_SETTER(maxb);
		LUNA_GETTER(minc);
		LUNA_SETTER(minc);
		LUNA_GETTER(maxc);
		LUNA_SETTER(maxc);
		LUNA_GETTER(mind);
		LUNA_SETTER(mind);
		LUNA_GETTER(maxd);
		LUNA_SETTER(maxd);
		LUNA_DECL(render);

	};

}

// The End.
