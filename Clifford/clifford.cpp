
#include "stdafx.h"

#include "../attractors.h"
#include "clifford.h"

using namespace attractors;
using namespace attractors::clifford;

void clifford::render(
	int quote, int pool,
	const geometry& g,
	image& img,
	color(*hue)(number),
	std::function<void()> notify) {

	color* bmp = img.data();
	int nx = img.width, ny = img.height;
	int fpc = g.frames / 100;

	number dX = nx / (g.maxX - g.minX), dY = ny / (g.maxY - g.minY);

	int i1 = quote * g.frames / pool, i2 = (quote + 1) * g.frames / pool;
	for (int i = i1; i < i2; i++) {
		const number p = static_cast<number>(i) / g.frames;
		const number a = cos(g.minA + p * (g.maxA - g.minA)) * 2.0;
		const number b = cos(g.minB + p * (g.maxB - g.minB)) * 2.0;
		const number c = cos(g.minC + p * (g.maxC - g.minC)) * 2.0;
		const number d = cos(g.minD + p * (g.maxD - g.minD)) * 2.0;

		const color curcol = hue(p);

		number x = 0.0, y = 0.0;
		number xn, yn;

		for (int j = 0; j < g.iters; j++) {
			xn = sin(a * y) + c * cos(a * x);
			yn = sin(b * x) + d * cos(b * y);
			x = xn;
			y = yn;

			if (j < g.skip)
				continue;

			int xi = static_cast<int>((x - g.minX) * dX);
			int yi = static_cast<int>((y - g.minY) * dY);
			if (xi >= 0 && xi < nx && yi >= 0 && yi < ny) {
				// there is no significant threads access to one data point at one time
				bmp[xi + yi * nx] += curcol;
			}
		}

		if (!(i % fpc)) {
			notify();
		}
	}
}

// The End.
