#pragma once
#include <string>

#include "mat_vec.h"


std::wstring vec3_to_wstr(Vec3f const &v);

struct TextToDraw
{
	std::wstring label;
	std::wstring data;
	int x;
	int y;
	int line_width;
	TextToDraw() {}
	TextToDraw(const wchar_t* chlabel, Vec3f const &v, int x = 10, int y = 0, int line_width = 20)
		: label(chlabel), data(vec3_to_wstr(v)), x(x), y(y), line_width(line_width) {}
	TextToDraw(const wchar_t* chlabel, size_t const &v, int x = 10, int y = 0, int line_width = 20)
		: label(chlabel), data(std::to_wstring(v)), x(x), y(y), line_width(line_width) {}
	TextToDraw(const wchar_t* chlabel, float const &v, int x = 10, int y = 0, int line_width = 20)
		: label(chlabel), data(std::to_wstring(v)), x(x), y(y), line_width(line_width) {}
	//if want to print another struct, we need to define new constructor

};

