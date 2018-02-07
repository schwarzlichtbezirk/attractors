
#include "stdafx.h"

#include <algorithm>
#include <fstream>

#include "attractors.h"

using namespace attractors;

/////////////////
// float color //
/////////////////

color color::hue(number h) {
	h *= 6.0;
	int hi = static_cast<int>(h);
	number hf = h - hi;

	switch (hi % 6) {
	case 0:
		return color(1.0, hf, 0.0);
	case 1:
		return color(1.0 - hf, 1.0, 0.0);
	case 2:
		return color(0.0, 1.0, hf);
	case 3:
		return color(0.0, 1.0 - hf, 1.0);
	case 4:
		return color(hf, 0.0, 1.0);
	case 5:
		return color(1.0, 0.0, 1.0 - hf);
	}

	return color();
}

color color::rainbow(number h) {
	h *= 6.0;
	int hi = static_cast<int>(h);
	number hf = h - hi;

	switch (hi % 6) {
	case 0:
		return color(0.0, hf, 1.0);
	case 1:
		return color(0.0, 1.0, 1.0 - hf);
	case 2:
		return color(hf, 1.0, 0);
	case 3:
		return color(1.0, 1.0 - hf, 0);
	case 4:
		return color(1.0, 0.0, hf);
	case 5:
		return color(1.0 - hf, 0.0, 1.0);
	}

	return color();
}

color color::gradient(number h, const color& hue1, const color& hue2) {
	h *= 6.0;
	int hi = static_cast<int>(h);
	number hf = h - hi;
	number df = 1. - h;
	return color(hue1.red*hf + hue2.red*df, hue1.green*hf + hue2.green*df, hue1.blue*hf + hue2.blue*df);
}

color color::monochrome(number h, const color& hue) {
	h *= 6.0;
	number hf = h - static_cast<int>(h);
	return color(hf*hue.red, hf*hue.green, hf*hue.blue);
}

color& color::operator+=(const color &rhs) {
	red += rhs.red;
	green += rhs.green;
	blue += rhs.blue;
	return *this;
}

color color::operator+(const color &rhs) const {
	return color(*this) += rhs;
}

void color::normalize() {
	number m = std::max<number>(red, std::max<number>(green, blue));
	if (m > 0) red /= m, green /= m, blue /= m;
	else red = green = blue = 1.0;
}

/////////////////
// image class //
/////////////////

image::image(int w, int h) :
	nx(w), ny(h),
	bitmap(w*h) { // allocate space for the primary image
}

void image::resize(int w, int h) {
	nx = w, ny = h;
	update();
}

void image::clear() {
	memset(bitmap.data(), 0, bitmap.size() * sizeof(color));
}

void image::writetga(const wchar_t* filename, number sensitivity) {
	std::ofstream os(filename, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	tgaheader(os, nx, ny, "Clifford attractors");

	// Raw uncompressed bytes
	for (const auto& c : bitmap) {
		os.put(static_cast<byte>((1.0 - exp(-sensitivity * c.blue)) * 255.0));
		os.put(static_cast<byte>((1.0 - exp(-sensitivity * c.green)) * 255.0));
		os.put(static_cast<byte>((1.0 - exp(-sensitivity * c.red)) * 255.0));
	}

	tgafooter(os);
	os.close();
}

void image::update() {
	bitmap.resize(nx*ny);
}

////////////////
// TGA saving //
////////////////

// TGA header, endian independent
// http://www.gamedev.ru/code/articles/TGA
void attractors::image::tgaheader(std::ostream& os, int wdh, int hgt, const char* id) {
	word xorg = 0, yorg = 0;
	os.put((byte)strlen(id)); // Length of ID
	os.put(0); // No colour map
	os.put(2); // uncompressed RGB
	os.put(0); os.put(0); // Index of colour map entry
	os.put(0); os.put(0); // Colour map length
	os.put(0); // Colour map size
	os.write((const char*)&xorg, 2); // X origin
	os.write((const char*)&yorg, 2); // Y origin
	os.write((const char*)&wdh, 2); // width
	os.write((const char*)&hgt, 2); // height
	os.put(24); // 24 bit bitmap
	os.put(0x00); // Description
	os.write(id, strlen(id));
}

// TGA empty footer
void attractors::image::tgafooter(std::ostream& os) {
	uint extoffset = 0, devoffset = 0;
	const char* signature = "TRUEVISION-XFILE.";
	os << extoffset << devoffset;
	os.write(signature, 18);
}

//////////////////////////
// geometry render base //
//////////////////////////

number geometry::rendermt(int pool, image& img, color::filter hue, bool notify) const {
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
			render(quote, pool, img, hue, notify ? (std::function<void()>)[&]() {
				auto pct = std::chrono::high_resolution_clock::now();
				auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(pct - pc1).count() / 1e9;
				percent++;
				mtxcout.lock(); // exclusive access to std::wcout
				std::wcout << L"\r" << percent << L"%, remains " << (percent > 0 ? (int)(dur * (100 - percent) / percent) : 0) << L"s   ";
				mtxcout.unlock();
			} : []() {});
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

	return std::chrono::duration_cast<std::chrono::nanoseconds>(pc2 - pc1).count() / 1e9;
}

// The End.
