
#include "stdafx.h"

#include "buddhabrot.h"

using namespace attractors;

std::atomic_ulong buddhabrot::numdiscard = 0;
std::atomic_ullong buddhabrot::nsum = 0;

void buddhabrot::render(
	int quote, int pool,
	image& img,
	color::filter hue,
	std::function<void()> notify) const {

	numdiscard = 0;
	nsum = 0;

	int i, j, n, ix, iy;
	double x, y, x0, y0, xnew, ynew;

	// Coordinates sequence
	std::vector<point> xyseq(nmax);
	point* seq = xyseq.data(); // for fast access

	color* bmp = img.data(); // for fast access
	int nx = img.nx, ny = img.ny;
	int frames = 1000000 * tmax;
	int fpc = frames / 100;

	std::vector<color> ct(nmax);
	color* ctp = ct.data();
	for (int i = 0; i < ct.size(); i++) {
		ctp[i] = hue((double)i / nmax);
	}

	// Make unique random sequence
	std::mt19937_64 prng(std::chrono::high_resolution_clock::now().time_since_epoch().count() + quote);
	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	// Iterate
	int i1 = quote * frames / pool, i2 = (quote + 1) * frames / pool;
	for (i = i1; i < i2; i++) {

		// Choose a random point in same range
		x0 = 6 * distribution(prng) - 3;
		y0 = 6 * distribution(prng) - 3;

		// Iterate the Mandelbrot and sets n to number of iterations before diverges from a starting point
		x = 0, y = 0;
		for (n = 0; n < nmax; n++) {
			xnew = x * x - y * y + x0;
			ynew = 2 * x * y + y0;
			seq[n].x = xnew;
			seq[n].y = ynew;
			// Determine state of this point, draw if it escapes
			if (xnew * xnew + ynew * ynew > 6.05) {
				for (j = 0; j < n; j++) {
					iy = (int)(ny * (0.5 - 1.0 / 3 * seq[j].x - 0.3*0.5));
					ix = (int)(nx * (0.5 + 1.0 / 3 * seq[j].y));
					if (ix >= 0 && iy >= 0 && ix < nx && iy < ny) {
						bmp[ix + iy * nx] += ctp[j];
					}
				}
				nsum.fetch_add(n);
				break;
			}
			x = xnew;
			y = ynew;
		}
		if (n == nmax) {
			numdiscard++;
		}

		if (!(i % fpc)) {
			notify();
		}

	}

}

// The End.
