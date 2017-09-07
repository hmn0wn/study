#pragma once
#define NOMINMAX
#include <Windows.h>


struct Color
{
	Color(float r_ = 0.f, float g_ = 0.f, float b_ = 0.f);
	Color(const Color &color);
	~Color();

	Color norm();

	const Color &operator=(const Color  &clr);
	Color operator*(float mult) const;
	Color operator*(const Color& clr) const;
	Color operator+(const Color & clr) const;
	Color &operator+=(const Color & clr);

	COLORREF get_win_rgb();


	Color inverse();
	Color clamp();

	float energy() 
	{}

public:
	float r, g, b;
};

namespace color_nm
{
	const Color black(0, 0, 0);
	const Color dark_grey = Color(20, 20, 20).norm();
	const Color too_dark_grey = Color(13, 13, 13).norm();


	const Color white(1.f, 1.f, 1.f);
	const Color red = Color(255, 0, 0).norm();
	const Color blue = Color(0, 0, 255).norm();
	const Color green = Color(0, 175, 0).norm();
	const Color violet = Color(140, 0, 255).norm();
	const Color yellow = Color(255, 255, 0).norm();

	const Color gold_ambient = Color(0.24725f, 0.1995f, 0.0745f);
	const Color gold_diffuse = Color(0.75164f, 0.60648f, 0.22648f);
	const Color gold_specular = Color(0.628f, 0.556f, 0.366f);
	const float gold_specular_exponent = 20.f;


	const Color chrome_ambient = Color(0.25f, 0.25f, 0.25f);
	const Color chrome_diffuse = Color(0.4f, 0.4f, 0.4f);
	const Color chrome_specular = Color(0.774597f, 0.774597f, 0.774597f);
	const float chrome_specular_exponent = 76.8f;

}