/*
	xn+1 = sin(a yn) + c cos(a xn)
	yn+1 = sin(b xn) + d cos(b yn)
*/

#include "stdafx.h"

#include "../../attractors.h"
#include "../clifford.h"

// Threads pool default size
#define POOL 4

using namespace attractors;

color Hue1(0.8, 0.25, 1.0), Hue2(0.25, 1.0, 0.0);

color gradient(number h) {
	return color::gradient(h, Hue1, Hue2);
}

color monochrome(number h) {
	return color::monochrome(h, Hue1);
}

int __cdecl wmain(int argc, wchar_t *argv[]) {
	using namespace attractors::clifford;

	// Change params only in this block
#pragma region

	static int pool = 0; // setup it later
	static int nImg = 0;
	static wchar_t filemask[_MAX_PATH] = L"img%04i.tga";
	static int width = 1600;
	static int height = 1200;
	static int frames = 10000;
	static int iters = 10000;
	static int skip = 10;
	static number sensitivity = 0.02;
	std::vector<geometry> geom;

	color(*fColorFilter)(number) = color::hue;
	number minX = -4.0, maxX = 4.0;

	std::wifstream is(argc > 1 ? argv[1] : L"ca_param.txt");
	if (is.fail()) {
		std::wcout <<
			L"no data file was opened.\n"
			L"pass data filename as command line argument,\n"
			L"or place \"ca_param.txt\" into current folder.\n"
			L"processing failed.\n";
		return 1;
	}
	static wchar_t line[128];
	is.getline(line, _countof(line), L'\n'); // Clifford attractors
	if (_wcsnicmp(line, L"Clifford attractors", 19)) {
		std::wcout <<
			L"file has not \"Clifford attractors\" signature.\n"
			L"processing failed.\n";
		return 2;
	}
	is.getline(line, _countof(line)); // Threads pool size:
	is >> pool;
	is.getline(line, _countof(line));
	is.getline(line, _countof(line)); // Number of first image and filename mask:
	is.getline(line, _countof(line)); // Number   Mask
	is >> nImg >> filemask;
	is.getline(line, _countof(line));
	is.getline(line, _countof(line)); // Parameters for all images:
	is.getline(line, _countof(line)); // width    height    frames    iters     sensitivity
	is >> width >> height;
	is >> frames >> iters >> sensitivity;
	is.getline(line, _countof(line));
	is.getline(line, _countof(line)); // Color filter, can be one of followed - "hue", "rainbow", "gradient", "mono":
	std::wstring colfilter;
	is >> colfilter;
	if (colfilter == L"hue") fColorFilter = color::hue;
	else if (colfilter == L"rainbow") fColorFilter = color::rainbow;
	else if (colfilter == L"gradient") fColorFilter = gradient;
	else if (colfilter == L"mono") fColorFilter = monochrome;
	is.getline(line, _countof(line));
	is.getline(line, _countof(line)); // Gradient colors components (for filters):
	is.getline(line, _countof(line)); // red      green     blue
	is >> Hue1.r >> Hue1.g >> Hue1.b;
	Hue1.normalize();
	is >> Hue2.r >> Hue2.g >> Hue2.b;
	Hue2.normalize();
	is.getline(line, _countof(line));
	is.getline(line, _countof(line)); // Horizontal Cartesian dimensions (vertical will been cast to image):
	is.getline(line, _countof(line)); // min(X)   max(X)
	is >> minX >> maxX;
	is.getline(line, _countof(line));
	is.getline(line, _countof(line)); // Coefficients ranges for functions:
	is.getline(line, _countof(line)); //    xn+1 = sin(a yn) + c cos(a xn)
	is.getline(line, _countof(line)); //    yn+1 = sin(b xn) + d cos(b yn)
	is.getline(line, _countof(line)); // min(a)   max(a)    min(b)    max(b)    min(c)    max(c)    min(d)    max(d)    

	while (!is.eof()) {
		geometry g;
		g.frames = frames;
		g.iters = iters;
		g.skip = skip;
		is >> g.minA; g.minA = acos(g.minA / 2.0);
		is >> g.maxA; g.maxA = acos(g.maxA / 2.0);
		is >> g.minB; g.minB = acos(g.minB / 2.0);
		is >> g.maxB; g.maxB = acos(g.maxB / 2.0);
		is >> g.minC; g.minC = acos(g.minC / 2.0);
		is >> g.maxC; g.maxC = acos(g.maxC / 2.0);
		is >> g.minD; g.minD = acos(g.minD / 2.0);
		is >> g.maxD; g.maxD = acos(g.maxD / 2.0);
		if (is.eof()) break; // if line reading not complete
		g.minX = minX;
		g.minY = g.minX * height / width;
		g.maxX = maxX;
		g.maxY = g.maxX * height / width;
		geom.push_back(g);
		is.getline(line, _countof(line)); // go to next line
	}
	is.close();

	std::wcout << L"total images: " << geom.size() << std::endl;
	std::wcout << L"width = " << width << L", height = " << height << L", 24 bits RGB" << std::endl;
	std::wcout << L"color filter: " << colfilter << std::endl;
	std::wcout << L"rendering frames for each = " << frames << std::endl;
	std::wcout << L"rendering iterations per frame = " << iters << std::endl;
	std::wcout << L"sensitivity = " << sensitivity << std::endl;

	if (!pool) {
		pool = std::thread::hardware_concurrency() > 2 ? POOL : 1;
	}

#pragma endregion

	// The density plot
	image img(width, height); // allocate space for the primary image

	for (const auto& g : geom) {
		img.clear();

		std::atomic_int percent = -pool; // skip calculation on each thread start
		std::atomic_int busynum = 0;
		std::mutex mtxcout;
		std::vector<std::thread> job(pool);
		std::mutex mtxpool;
		std::condition_variable cv;

		// Rendering
		auto pc1 = std::chrono::high_resolution_clock::now();
		for (int quote = 0; quote < pool; quote++) {
			busynum++;
			job[quote] = std::thread([&, quote]() {
				render(quote, pool, g, img, fColorFilter, [&]() {
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

		// Generate new filename
		static wchar_t filename[_MAX_PATH];
		swprintf_s(filename, _countof(filename), filemask, nImg);

		// Save image
		std::wcout << L"writing...";
		img.writetga(filename, sensitivity);
		std::wcout << L"\r" << filename << L"      " << std::endl;

		nImg++;
	}

	// wait until key pressed
	std::wcout << L"processing complete." << std::endl;

	return 0;
}

// The End.
