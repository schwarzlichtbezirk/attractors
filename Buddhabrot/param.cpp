#include "stdafx.h"

#include "../attractors.h"
#include "buddhabrot.h"

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

static int pool = 0; // setup it later
const wchar_t* filename = L"buddhabrot.tga";
static int nx = NX, ny = NY;
static number sensitivity = 0.02;
static buddhabrot g;

struct {
	const wchar_t* id;
	int len;
	const wchar_t* type;
	const wchar_t* dscr;
	void(*hnd)(wchar_t* arg);
} flags[] = {
	{
		L"pool", 4, L"number",
		L"working threads pool size, default is 4 for multicore processors.",
		[](wchar_t* arg) {
			g.tmax = _wtoi(arg);
		}
	},
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
			g.nmax = _wtoi(arg);
		}
	},
	{
		L"tmax", 4, L"number",
		L"number of iterations, multiples of 1 million, default is 50.",
		[](wchar_t* arg) {
			g.tmax = _wtoi(arg);
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
	g.nmax = NMAX, g.tmax = TMAX;
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

	int frames = 1000000 * g.tmax;
	number dur = g.rendermt(pool, img, color::rainbow, true);
	std::wcout << L"\r" << dur << L"s            " << std::endl;
	std::wcout << L"discard points: " << (100. * buddhabrot::numdiscard / frames) << L"%" << std::endl;
	std::wcout << L"average bailout: " << ((number)buddhabrot::nsum / (frames - buddhabrot::numdiscard)) << std::endl;
	std::wcout << L"total iterations: " << (buddhabrot::nsum / 1e6) << L"M" << std::endl;

	// Save image
	std::wcout << L"writing...";
	img.writetga(filename, sensitivity);
	std::wcout << L"\r" << filename << L"      " << std::endl;

	return 0;
}

// The End.
