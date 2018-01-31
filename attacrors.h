#pragma once

namespace attractors {

	typedef double number;

	class color {
	public:

		static color createHueColor(number h);
		static color createHueGradient(number h);
		static color createHueMask(number h);
		static color createHueMono(number h);
		static color createGradient(number h, const color& hue1, const color& hue2);

		color(const number &red = 0, const number &green = 0, const number &blue = 0) : r(red), g(green), b(blue) {}

		color& operator+=(const color &rhs);
		void add(const color &rhs);

		color operator+(const color &rhs) const;

		void Normalize();

	public:

		number r, g, b;

		static color Hue1, Hue2;
	};

}

// The End.
