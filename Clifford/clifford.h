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
			color* image, // image bitmap
			int width, int height, // image dimension
			color(*hue)(number), // Hue color generator
			std::function<void()> notify // notify called with each per cent comlete and breaks if it's return false
		);
		void writetga(const wchar_t* filename, const color* image, int width, int height, number sensitivity);
	}
}

// The End.
