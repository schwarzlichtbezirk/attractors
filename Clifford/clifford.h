#ifndef _CLIFFORD_
#define _CLIFFORD_

//-----------------------------------------------------------------------------

#include <functional>

namespace attractors {
	namespace clifford {
		struct geometry {
			int frames, iters, skip;
			number minX, minY, maxX, maxY;
			number minA, maxA, minB, maxB, minC, maxC, minD, maxD;
		};

		void render(
			int quote, int pool,
			const geometry& g, // geometry equation parameters
			image& img, // plotting image
			color(*hue)(number), // Hue color generator
			std::function<void()> notify // notify called with each per cent comlete and breaks if it's return false
		);
	}
}

//-----------------------------------------------------------------------------

#endif // _CLIFFORD_

// The End.
