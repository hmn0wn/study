#include "stdafx.h"
#include "color.h"

Color::Color(float r_, float g_, float b_) :
	r(r_), g(g_), b(b_)
{}

Color::Color(const Color & color):
	r(color.r), g(color.g), b(color.b)
{}

const Color & Color::operator=(const Color & clr)
{
	r = clr.r;
	g = clr.g;
	b = clr.b;
	return *this;
}

Color Color::norm()
{
	return Color(r / 255.0f, g / 255.0f, b / 255.0f);
}

Color Color::operator*(float mult) const
{
	return Color(mult * r, mult * g, mult * b);
}

Color Color::operator*(const Color & clr) const
{
	return Color(r * clr.r, g * clr.g, b * clr.b);
}

Color Color::operator+(const Color & clr) const
{
	return Color(r + clr.r, g + clr.g, b + clr.b);
}

Color & Color::operator+=(const Color & clr)
{
	r += clr.r;
	g += clr.g;
	b += clr.b;
	return *this;
}

COLORREF Color::get_win_rgb()
{
	return RGB(r * 255, g * 255, b * 255);
}

Color::~Color()
{
}

Color Color::inverse()
{
	return Color(1.0f - r, 1.0f - g, 1.0f - b);
}

Color Color::clamp()
{
	float cr = r, cg = g, cb = b;
	if (r < 0.f)
		cr = 0.f;
	else if (r > 1.f)
		cr = 1.f;

	if (g < 0.f)
		cg = 0.f;
	else if (r > 1.f)
		cg = 1.f;

	if (b < 0.f)
		cb = 0.f;
	else if (b > 1.f)
		cb = 1.f;

	return Color(cr, cg, cb);
}
