
#include "stdafx.h"

#include <fstream>

#include "../attractors.h"
#include "clifford.h"

using namespace attractors;
using namespace attractors::clifford;

void clifford::render(
	int quote, int pool,
	const geometry& g,
	color* image,
	int width, int height,
	color(*hue)(number),
	std::function<void()> notify) {

	number dX = width / (g.maxX - g.minX), dY = height / (g.maxY - g.minY);

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
			if (xi >= 0 && xi < width && yi >= 0 && yi < height) {
				// there is no significant threads access to one data point at one time
				image[xi + (height - yi) * width] += curcol;
			}
		}
		if (!(i % (g.frames / 100))) {
			notify();
		}
	}
}

void clifford::writetga(const wchar_t* filename, const color* image, int width, int height, number sensitivity) {
	std::ofstream os(filename, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	tgaheader(os, width, height, "Clifford attractors");

	// Raw uncompressed bytes
	for (int i = 0; i < width*height; i++) {
		const auto& c = image[i];
		os.put(static_cast<byte>((1.0 - exp(-sensitivity * c.b)) * 255.0));
		os.put(static_cast<byte>((1.0 - exp(-sensitivity * c.g)) * 255.0));
		os.put(static_cast<byte>((1.0 - exp(-sensitivity * c.r)) * 255.0));
	}

	tgafooter(os);
	os.close();
}

// The End.
