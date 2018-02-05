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

// Threads pool default size
#define POOL 4

using namespace attractors;

typedef struct {
	double x, y;
} XY;

static int nx = NX, ny = NY;
static number sensitivity = 0.02;
static int nmax = NMAX, tmax = TMAX;
static int pool = 0; // setup it later

std::atomic_ulong numdiscard = 0;
std::atomic_ullong nsum = 0;

// Output TGA-image file name
const wchar_t* filename = L"buddhabrot.tga";

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

void render(
	int quote, int pool,
	image& img,
	color(*hue)(number),
	std::function<void()> notify) {

	int i, j, n, ix, iy;
	double x, y, x0, y0, xnew, ynew;

	// Coordinates sequence
	std::vector<XY> xyseq(nmax);
	XY* seq = xyseq.data(); // for fast access

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
	image img(nx, ny);
	img.clear();

	int frames = 1000000 * tmax;

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
			render(quote, pool, img, color::rainbow, [&]() {
				auto pct = std::chrono::high_resolution_clock::now();
				auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(pct - pc1).count() / 1e9;
				percent++;
				mtxcout.lock(); // exclusive access to std::wcout
				std::wcout << L"\r" << percent << L"%, remains " << (percent > 0 ? (int)(dur * (100 - percent) / percent) : 0) << L"s   ";
				mtxcout.unlock();
			});
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
	std::wcout << L"discard points: " << (100. * numdiscard / frames) << L"%" << std::endl;
	std::wcout << L"average bailout: " << ((double)nsum / (frames - numdiscard)) << std::endl;
	std::wcout << L"total iterations: " << (nsum / 1e6) << L"M" << std::endl;

	// Save image
	std::wcout << L"writing...";
	img.writetga(filename, sensitivity);
	std::wcout << L"\r" << filename << L"      " << std::endl;

	return 0;
}

// The End.
