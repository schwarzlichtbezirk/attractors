#ifndef __buddhabrot_h__
#define __buddhabrot_h__

//-----------------------------------------------------------------------------

#include <functional>
#include "../attractors.h"

namespace attractors {
	struct buddhabrot {
		int nmax, tmax;

		static std::atomic_ulong numdiscard;
		static std::atomic_ullong nsum;

		void render(
			int quote, int pool,
			image& img, // plotting image
			color(*hue)(number), // Hue color generator
			std::function<void()> notify // notify called with each per cent comlete
		) const;
	};
}

//-----------------------------------------------------------------------------

#endif // __buddhabrot_h__

// The End.
