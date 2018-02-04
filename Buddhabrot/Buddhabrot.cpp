#include "stdafx.h"

#include "../attractors.h"

/*
   Crude, but functional, little program to calculate the
   So called "buddha mandelbrot" or "buddhabrot"
   Please note that this is not supposed to be the most efficient
   implementation, it is only intended to be a simple example with
   plenty of scope for improvement by the reader.
*/

// Image dimensions
#define NX 1000
#define NY 1000

// Length of sequence to test escape status
// Also known as bailout
#define NMAX 200

// Number of iterations, multiples of 1 million
#define TMAX 50

// Color algorithm
#define COLORALG 2

// Threads pool default size
#define POOL 4

using namespace attractors;

typedef struct {
	double x, y;
} XY;

static int nx = NX, ny = NY;
static int nmax = NMAX, tmax = TMAX;
static int pool = 0; // setup it later

std::atomic_ulong numdiscard = 0;
std::atomic_ullong nsum = 0;

#if COLORALG == 2
// Gradient colors
color hue1(1.0, 1.0, 0.3), hue2(0, 0, 0.5);
#endif

// Output TGA-image file name
const wchar_t* filename = L"buddhabrot.tga";

// Prototypes
void writetga(const wchar_t*, const planecolor*, int, int);
void bailout(double, double, int&, XY*);

struct {
	const wchar_t* id;
	int len;
	const wchar_t* type;
	const wchar_t* dscr;
	void(*hnd)(wchar_t* arg);
} flags[] = {
	{
		L"fn", 1, L"string",
		L"output tga-image file name, default is \"buddhabrot.tga\".",
		[](wchar_t* arg) {
			filename = arg;
		}
	},
	{
		L"nx", 2, L"number",
		L"image width, in pixels, default is 1000.",
		[](wchar_t* arg) {
			nx = _wtoi(arg);
		}
	},
	{
		L"ny", 2, L"number",
		L"image height, in pixels, default is 1000.",
		[](wchar_t* arg) {
			ny = _wtoi(arg);
		}
	},
	{
		L"sq", 2, L"number",
		L"image square width and height (alternative for nx/ny), in pixels.",
		[](wchar_t* arg) {
			nx = ny = _wtoi(arg);
		}
	},
	{
		L"nmax", 4, L"number",
		L"length of sequence to test escape status (also known as bailout), default is 500.",
		[](wchar_t* arg) {
			nmax = _wtoi(arg);
		}
	},
	{
		L"tmax", 4, L"number",
		L"number of iterations, multiples of 1 million, default is 50.",
		[](wchar_t* arg) {
			tmax = _wtoi(arg);
		}
	},
	{
		L"pool", 4, L"number",
		L"working threads pool size, default is 4 for multicore processors.",
		[](wchar_t* arg) {
			tmax = _wtoi(arg);
		}
	},
};

void cmdlinehelp() {
	std::wcout << L"command line parameters usage:" << std::endl;
	for (const auto& f : flags) {
		std::wcout << L"  -" << f.id << L" " << f.type << std::endl;
		std::wcout << L"      " << f.dscr << std::endl;
	}
}

int __cdecl wmain(int argc, wchar_t *argv[]) {
	// process command line parameters
	for (int param = 1; param < argc; param++) {
		auto arg = argv[param];
		if (*arg == L'-') {
			arg++;
			if (*arg == L'-') {
				arg++;
			}
			bool found = false;
			for (const auto& f : flags) {
				if (!_wcsnicmp(arg, f.id, f.len)) {
					arg += f.len;
					if (*arg = L'=') {
						arg++;
					}
					if (!*arg && param < argc - 1) {
						param++;
						arg = argv[param];
					}
					if (*arg) {
						f.hnd(arg);
					}
					found = true;
					break;
				}
			}
			if (!found) {
				if ((*arg < L'A' || *arg > L'Z') && (*arg < L'a' || *arg > L'z')) {
					std::wcout << L"bad flag syntax: " << arg << std::endl;
				}
				else if (_wcsnicmp(arg, L"h", 1) && _wcsnicmp(arg, L"help", 4)) {
					std::wcout << L"flag provided but not defined: " << std::endl;
				}
				cmdlinehelp();
				return 0;
			}
		}
		else {
			std::wcout << L"unprocessed parameter: " << arg << std::endl;
			cmdlinehelp();
			return 0;
		}
	}

	if (!pool) {
		pool = std::thread::hardware_concurrency() > 2 ? POOL : 1;
	}

	// The density plot
	std::vector<planecolor> image(nx*ny); // allocate space for the primary image
	planecolor* imptr = image.data(); // for fast access
	memset(imptr, 0, image.size() * sizeof(planecolor)); // clear to black

	int frames = 1000000 * tmax;
	int fpc = frames / 100;

	std::atomic_int percent = -pool; // skip calculation on each thread start
	std::atomic_int busynum = 0;
	std::mutex mtxcout;
	std::vector<std::thread> job(pool);
	std::mutex mtxpool;
	std::condition_variable cv;

	auto pc1 = std::chrono::high_resolution_clock::now();
	for (int quote = 0; quote < pool; quote++) {
		busynum++;
		job[quote] = std::thread([&, quote]() {
			int i, j, n, ix, iy;
			double x, y, x0, y0, xnew, ynew;

			// Coordinates sequence
			std::vector<XY> xyseq(nmax);
			XY* seq = xyseq.data(); // for fast access

			// Make unique random sequence
			std::mt19937_64 prng(pc1.time_since_epoch().count() + quote);
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
								(std::atomic_short*)imptr[iy * nx + ix]++;
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
					auto pct = std::chrono::high_resolution_clock::now();
					auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(pct - pc1).count() / 1e9;
					percent++;
					mtxcout.lock(); // exclusive access to std::wcout
					std::wcout << L"\r" << percent << L"%, remains " << (percent > 0 ? (int)(dur * (100 - percent) / percent) : 0) << L"s   ";
					mtxcout.unlock();
				}

			}
			busynum--;
			cv.notify_one();
		});
	}
	std::unique_lock<std::mutex> lck(mtxpool);
	cv.wait(lck, [&]()->bool { return busynum == 0; });
	for (auto& th : job) {
		th.join();
	}
	auto pc2 = std::chrono::high_resolution_clock::now();
	std::wcout << L"\r" << std::chrono::duration_cast<std::chrono::nanoseconds>(pc2 - pc1).count() / 1e9 << L"s            " << std::endl;
	std::wcout << L"discard points: " << (numdiscard / tmax / 1e4) << L"%" << std::endl;
	std::wcout << L"average bailout: " << (nsum / (tmax * 1e6 - numdiscard)) << std::endl;
	std::wcout << L"total iterations: " << (nsum / 1e6) << L"M" << std::endl;

	// Save the result 
	writetga(filename, imptr, nx, ny);

	return 0;
}

/*
   Write the buddha image to a minimal TGA file.
   Can be opened with gimp, PhotoShop, etc.
*/
void writetga(const wchar_t* filename, const planecolor* image, int width, int height) {
	// Find the largest density value
	planecolor biggest = 0, smallest = (planecolor)-1;
	for (int i = 0; i < width*height; i++) {
		biggest = std::max<planecolor>(biggest, image[i]);
		smallest = std::min<planecolor>(smallest, image[i]);
	}

	// Write the image
	std::wcout << L"density value range: " << smallest << L" to " << biggest << std::endl;
	std::wcout << L"writing...";

	std::ofstream os(filename, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	tgaheader(os, width, height, "Buddhabrot attractor");

	// Raw uncompressed bytes
	double ramp;
	for (int i = 0; i < width*height; i++) {
		ramp = (double)(image[i] - smallest) / (biggest - smallest);
		ramp = pow(3.0*ramp, 1.0 / 3);
		if (ramp > 1) ramp = 1;
#if COLORALG == 1
		const double bound = 0.4;
		ramp = (double)(image[i] - smallest) / (biggest - smallest);
		ramp = sqrt(3.0*ramp);
		if (ramp > 1) ramp = 1;
		if (ramp > bound) {
			ramp = (ramp - bound) / (1.0 - bound);
			os.put((byte)(0));
			os.put((byte)(ramp * 255));
			os.put((byte)(ramp*255));
		} else {
			ramp = (bound - ramp) / bound;
			os.put((byte)(ramp * 255));
			os.put((byte)(0));
			os.put((byte)(ramp*255));
		}
#elif COLORALG == 2
		auto c = color::createGradient(ramp, hue1, hue2);
		os.put((byte)(c.b * 255));
		os.put((byte)(c.g * 255));
		os.put((byte)(c.r * 255));
#else
		os.put((byte)(0));
		os.put((byte)(ramp * 255));
		os.put((byte)(ramp * 255));
#endif
	}

	tgafooter(os);
	os.close();

	std::wcout << L"\r" << filename << L"      " << std::endl;
}

// The End.
