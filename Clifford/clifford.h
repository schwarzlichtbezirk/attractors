#pragma once
#ifndef __clifford_h__
#define __clifford_h__

//-----------------------------------------------------------------------------

#include <functional>
#include "../attractors.h"

namespace attractors {
	struct clifford : public geometry {
		int frames, iters, skip;
		number minX, maxX, minY, maxY;
		number minA, maxA, minB, maxB, minC, maxC, minD, maxD;

		void render(
			int quote, int pool,
			image& img, // plotting image
			color::filter hue, // hue color generator
			std::function<void()> notify // notify called with each per cent comlete
		) const override;
	};
}

//-----------------------------------------------------------------------------

#endif // __clifford_h__

// The End.
