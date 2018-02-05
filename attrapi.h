#pragma once

#include "Luna.h"
#include "attractors.h"

namespace attractors {

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

}

// The End.
