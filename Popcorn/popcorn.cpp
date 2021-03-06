/*
xn+1 = xn - h sin(yn + tan(k yn))
yn+1 = yn - h sin(xn + tan(k xn))
*/

#include "stdafx.h"

#include "popcorn.h"

using namespace attractors;

color hue1(1.0, 0, 0), hue2(0, 0, 1.0);

void popcorn::render(
	int quote, int pool,
	image& img,
	color::filter hue,
	std::function<void()> notify) const {

	color* bmp = img.data();
	int nx = img.nx, ny = img.ny;
	int fpc = frames / 100;

	// Make unique random sequence
	std::mt19937_64 prng(std::chrono::high_resolution_clock::now().time_since_epoch().count() + quote);
	std::uniform_real_distribution<number> distribution(0.0, 1.0);

	number dX = nx / (maxX - minX), dY = ny / (maxY - minY);

	int i1 = quote * frames / pool, i2 = (quote + 1) * frames / pool;
	for (int i = i1; i < i2; i++) {
		const number p = static_cast<number>(i) / frames;
		const color curcol = hue(p);

		number x = distribution(prng);
		number y = distribution(prng);
		number xn, yn;


		for (int j = 0; j < iters; j++) {
			xn = x - h * sin(y + tan(k * y));
			yn = y - h * sin(x + tan(k * x));
			x = xn;
			y = yn;

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
