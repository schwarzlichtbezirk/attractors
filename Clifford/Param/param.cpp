/*
	xn+1 = sin(a yn) + c cos(a xn)
	yn+1 = sin(b xn) + d cos(b yn)
*/

#include "stdafx.h"

#include <conio.h>

#include "../../attractors.h"
#include "../clifford.h"

// Threads pool default size
#define POOL 4

using namespace attractors;

int __cdecl wmain(void) {
	// Change params only in this block
#pragma region

	static int pool = 0; // setup it later
	static wchar_t filename[_MAX_PATH] = L"clifford.tga";
	static int width = 1600;
	static int height = 1200;
	static number sensitivity = 0.02;
	static clifford g;

	g.frames = 10000;
	g.iters = 10000;
	g.skip = 10;

	std::wcout << L"threads pool = "; std::wcin >> pool;
	std::wcout << L"image filename: "; std::wcin.getline(filename, _countof(filename));
	std::wcout << L"image width = "; std::wcin >> width;
	std::wcout << L"image height = "; std::wcin >> height;
	std::wcout << L"rendering frames = "; std::wcin >> g.frames;
	std::wcout << L"rendering iterations per frame = "; std::wcin >> g.iters;
	std::wcout << L"sensitivity = "; std::wcin >> sensitivity;
	std::wcout << L"Enter coefficients ranges for functions:" << std::endl
		<< L"   xn+1 = sin(a yn) + c cos(a xn)" << std::endl
		<< L"   yn+1 = sin(b xn) + d cos(b yn)" << std::endl;
	std::wcout << L"min(a) = "; std::wcin >> g.minA; g.minA = acos(g.minA / 2.0);
	std::wcout << L"max(a) = "; std::wcin >> g.maxA; g.maxA = acos(g.maxA / 2.0);
	std::wcout << L"min(b) = "; std::wcin >> g.minB; g.minB = acos(g.minB / 2.0);
	std::wcout << L"max(b) = "; std::wcin >> g.maxB; g.maxB = acos(g.maxB / 2.0);
	std::wcout << L"min(c) = "; std::wcin >> g.minC; g.minC = acos(g.minC / 2.0);
	std::wcout << L"max(c) = "; std::wcin >> g.maxC; g.maxC = acos(g.maxC / 2.0);
	std::wcout << L"min(d) = "; std::wcin >> g.minD; g.minD = acos(g.minD / 2.0);
	std::wcout << L"max(d) = "; std::wcin >> g.maxD; g.maxD = acos(g.maxD / 2.0);

	g.minX = -4.0;
	g.minY = g.minX * height / width;

	g.maxX = 4.0;
	g.maxY = g.maxX * height / width;

	if (!pool) {
		pool = std::thread::hardware_concurrency() > 2 ? POOL : 1;
	}

#pragma endregion

	// The density plot
	image img(width, height);
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
			g.render(quote, pool, img, color::hue, [&]() {
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

	// Save image
	std::wcout << L"writing...";
	img.writetga(filename, sensitivity);
	std::wcout << L"\r" << filename << L"      " << std::endl;

	// wait until key pressed
	std::wcout << L"all complete" << std::endl << L"press any key" << std::endl;
	_getch();

	return 0;
}

// The End.
