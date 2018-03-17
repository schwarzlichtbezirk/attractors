#pragma once
#ifndef __popcorn_h__
#define __popcorn_h__

//-----------------------------------------------------------------------------

#include <functional>
#include "../attractors.h"

namespace attractors {
	struct popcorn : public geometry {
		int frames, iters;
		number h, k;
		number minX, maxX, minY, maxY;

		void render(
			int quote, int pool,
			image& img, // plotting image
			color::filter hue, // hue color generator
			std::function<void()> notify // notify called with each per cent comlete
		) const override;
	};
}

//-----------------------------------------------------------------------------

#endif // __popcorn_h__

// The End.
