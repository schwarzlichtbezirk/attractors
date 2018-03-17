/*
xn+1 = sin(a yn) + c cos(a xn)
yn+1 = sin(b xn) + d cos(b yn)
*/

#include "stdafx.h"

#include "clifford.h"

using namespace attractors;

void clifford::render(
	int quote, int pool,
	image& img,
	color::filter hue,
	std::function<void()> notify) const {

	color* bmp = img.data();
	int nx = img.nx, ny = img.ny;
	int fpc = frames / 100;

	number dX = nx / (maxX - minX), dY = ny / (maxY - minY);

	int i1 = quote * frames / pool, i2 = (quote + 1) * frames / pool;
	for (int i = i1; i < i2; i++) {
		const number p = static_cast<number>(i) / frames;
		const number a = cos(minA + p * (maxA - minA)) * 2.0;
		const number b = cos(minB + p * (maxB - minB)) * 2.0;
		const number c = cos(minC + p * (maxC - minC)) * 2.0;
		const number d = cos(minD + p * (maxD - minD)) * 2.0;

		const color curcol = hue(p);

		number x = 0.0, y = 0.0;
		number xn, yn;

		for (int j = 0; j < iters; j++) {
			xn = sin(a * y) + c * cos(a * x);
			yn = sin(b * x) + d * cos(b * y);
			x = xn;
			y = yn;

			if (j < skip)
				continue;

			int xi = static_cast<int>((x - minX) * dX);
			int yi = static_cast<int>((y - minY) * dY);
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
