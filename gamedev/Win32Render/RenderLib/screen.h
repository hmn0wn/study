#pragma once
#define NOMINMAX
#include <Windows.h>
#include <cstddef>
#include <utility>
#include <math.h>

#include "color.h"
#include "mat_vec.h"

struct Screen
{
	
	Screen();
	~Screen();

	Screen(
		HDC hdc,
		int width,
		int height,
		Color background);

	Screen &operator=(Screen &s);

	void set_rgb_pixel(int x, int y, Color const& pixel_color);
	
	void print_text(int x, int y, LPCWSTR info);

	void draw_buffer();

	void draw_background();

	void update_param(HDC hdc, int width, int height);

	float get_aspect_ratio();

	void draw_line(int x0, int y0, int x1, int y1, Color color); //Brezenhime alg

	void draw_line(float x0, float y0, float x1, float y1, Color color);


	HDC get_hdc();

	int get_width();

	int get_height();

	void create_buf();
	void create_zbuf();

	void clear_zbuf();

	void set_zbuf_depth(int x, int y, float depth);

	float get_zbuf_detpth(int x, int y);


private:
	HDC hdc;
	int width, height;
	BYTE * image_buf;
	float   *z_buf;
	size_t buf_size;
	size_t zbuf_size;
	Color background;
};
