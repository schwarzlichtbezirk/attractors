
#include "stdafx.h"

#include <algorithm>

#include "attractors.h"

using namespace attractors;

/////////////////
// float color //
/////////////////

color color::Hue1(0.8, 0.25, 1.0), color::Hue2(0.25, 1.0, 0.0);

color color::createHueColor(number h) {
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

color color::createHueGradient(number h) {
	h *= 6.0;
	int hi = static_cast<int>(h);
	number hf = h - hi;
	return createGradient(hf, Hue1, Hue2);
}

color color::createHueMask(number h) {
	h *= 6.0;
	int hi = static_cast<int>(h);
	number hf = h - hi;

	switch (hi % 6) {
	case 0:
		return color(1.0*Hue1.r, hf*Hue1.g, 0.0);
	case 1:
		return color((1.0 - hf)*Hue1.r, 1.0*Hue1.g, 0.0);
	case 2:
		return color(0.0, 1.0*Hue1.g, hf*Hue1.b);
	case 3:
		return color(0.0, (1.0 - hf)*Hue1.g, 1.0*Hue1.b);
	case 4:
		return color(hf*Hue1.r, 0.0, 1.0*Hue1.b);
	case 5:
		return color(1.0*Hue1.r, 0.0, (1.0 - hf)*Hue1.b);
	}

	return color();
}

color color::createGradient(number h, const color& hue1, const color& hue2) {
	number d = 1.0 - h;
	return color(hue1.r*h + hue2.r*d, hue1.g*h + hue2.g*d, hue1.b*h + hue2.b*d);
}

color color::createHueMono(number h) {
	h *= 6.0;
	number hf = h - static_cast<int>(h);
	return color(hf*Hue1.r, hf*Hue1.g, hf*Hue1.b);
}

color& color::operator+=(const color &rhs) {
	r += rhs.r;
	g += rhs.g;
	b += rhs.b;
	return *this;
}

color color::operator+(const color &rhs) const {
	return color(*this) += rhs;
}

void color::normalize() {
	number m = std::max<number>(r, std::max<number>(g, b));
	if (m > 0) r /= m, g /= m, b /= m;
	else r = g = b = 1.0;
}

/////////////////
// image class //
/////////////////

image::image(int w, int h) :
	width(w), height(h),
	bitmap(w*h) { // allocate space for the primary image
}

void image::resize(int w, int h) {
	width = w, height = h;
	bitmap.resize(w*h);
}

void image::clear() {
	memset(bitmap.data(), 0, bitmap.size() * sizeof(color));
}

////////////////
// TGA saving //
////////////////

// TGA header, endian independent
// http://www.gamedev.ru/code/articles/TGA
void attractors::tgaheader(std::ostream& os, int wdh, int hgt, const char* id) {
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
void attractors::tgafooter(std::ostream& os) {
	uint extoffset = 0, devoffset = 0;
	const char* signature = "TRUEVISION-XFILE.";
	os << extoffset << devoffset;
	os.write(signature, 18);
}

// The End.
