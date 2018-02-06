#pragma once
#ifndef __clifford_h__
#define __clifford_h__

//-----------------------------------------------------------------------------

#include <functional>
#include "../attractors.h"

namespace attractors {
	struct clifford {
		int frames, iters, skip;
		number minX, minY, maxX, maxY;
		number minA, maxA, minB, maxB, minC, maxC, minD, maxD;

		void render(
			int quote, int pool,
			image& img, // plotting image
			color(*hue)(number), // Hue color generator
			std::function<void()> notify // notify called with each per cent comlete
		) const;
	};
}

//-----------------------------------------------------------------------------

#endif // __clifford_h__

// The End.
