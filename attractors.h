#pragma once

#include <ostream>

namespace attractors {

	typedef unsigned __int8  byte;
	typedef unsigned __int16 word;
	typedef unsigned __int32 uint;
	typedef unsigned __int64 huge;
	typedef double number;
	typedef unsigned __int16 planecolor;

	class color {
	public:

		static color hue(number h);
		static color rainbow(number h);
		static color monochrome(number h, const color& hue);
		static color gradient(number h, const color& hue1, const color& hue2);

		color(const number &red = 0, const number &green = 0, const number &blue = 0) : r(red), g(green), b(blue) {}

		inline color& operator+=(const color &rhs);
		color operator+(const color &rhs) const;

		void normalize();

	public:

		number r, g, b;
	};

	struct image {
		int width, height;
		std::vector<color> bitmap;

		image(int w, int h);
		color* data() { return bitmap.data(); }
		const color* data() const { return bitmap.data(); }
		void resize(int w, int h);
		void clear(); // clear content
		void writetga(const wchar_t* filename, number sensitivity); // can be opened with gimp, PhotoShop, etc.
	};

	// TGA saving
	void tgaheader(std::ostream& os, int wdh, int hgt, const char* id = "");
	void tgafooter(std::ostream& os);

}

// The End.
