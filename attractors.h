#pragma once
#ifndef __attractors_h_
#define __attractors_h_

//-----------------------------------------------------------------------------

#include <ostream>

namespace attractors {

	typedef unsigned __int8  byte;
	typedef unsigned __int16 word;
	typedef unsigned __int32 uint;
	typedef unsigned __int64 huge;
	typedef double number;

	class color {
	public:

		typedef color(*filter)(number);
		static color hue(number h);
		static color rainbow(number h);
		static color monochrome(number h, const color& hue);
		static color gradient(number h, const color& hue1, const color& hue2);

		color(number r = 0, number g = 0, number b = 0) : red(r), green(g), blue(b) {}

		inline color& operator+=(const color &rhs);
		color operator+(const color &rhs) const;

		void normalize();

	public:

		number red, green, blue;
	};

	struct point {
		number x, y;
	};

	class image {
	public:

		image() = default;
		image(int w, int h);
		color* data() { return bitmap.data(); }
		const color* data() const { return bitmap.data(); }
		void resize(int w, int h);
		void clear(); // clear content
		void writetga(const wchar_t* filename, number sensitivity); // can be opened with gimp, PhotoShop, etc.

		int nx, ny;

		// TGA saving
		static void tgaheader(std::ostream& os, int wdh, int hgt, const char* id = "");
		static void tgafooter(std::ostream& os);

	protected:

		void update();

	private:

		std::vector<color> bitmap;
	};

	// geometry render interface
	struct geometry {
		virtual void render(
			int quote, int pool,
			image& img, // plotting image
			color::filter hue, // Hue color generator
			std::function<void()> notify // notify called with each per cent comlete
		) const = 0;

		// multithreaded render, returns duration in seconds
		number rendermt(int pool, image& img, color::filter hue, bool notify = true) const;
	};

}

//-----------------------------------------------------------------------------

#endif // __attractors_h_

// The End.
