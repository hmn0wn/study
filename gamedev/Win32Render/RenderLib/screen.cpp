#include "stdafx.h"
#include "screen.h"

Screen::Screen()
{
}

Screen::~Screen()
{
}

Screen::Screen(HDC hdc, int width, int height, Color background):
	hdc(hdc), width(width), height(height), background(background)
{
	
	image_buf = nullptr;
	z_buf = nullptr;
	create_buf();
	create_zbuf();
}

Screen & Screen::operator=(Screen & s)
{
	hdc = s.hdc;
	width = s.width;
	height = s.height;
	image_buf = s.image_buf;
	
	z_buf = s.z_buf;
	buf_size = s.buf_size;
	zbuf_size = s.zbuf_size;

	background = s.background;
	return *this;
}

void Screen::set_rgb_pixel(int x, int y, Color const & pixel_color)
{
	int index = (int(x) + int(y) * int(width)) * 4;
	
	image_buf[index + 2] = BYTE(pixel_color.r * 255.0);
	image_buf[index + 1] = BYTE(pixel_color.g * 255.0);
	image_buf[index + 0] = BYTE(pixel_color.b * 255.0);
}

void Screen::print_text(int x, int y, LPCWSTR info)
{
	COLORREF old_fcolor = SetTextColor(hdc, background.inverse().get_win_rgb());
	COLORREF old_bcolor = SetBkColor(hdc, background.get_win_rgb());

	
	TextOut(hdc, x, height - y, info, wcslen(info));//win screen O(0,0) - O(height,0) of standart cord sys

	SetTextColor(hdc, old_fcolor);
	SetBkColor(hdc, old_bcolor);
}

void Screen::draw_buffer()
{

	tagBITMAPINFO bif;
	bif.bmiHeader.biSize = sizeof(bif.bmiHeader);
	bif.bmiHeader.biWidth = width;
	bif.bmiHeader.biHeight = height; //subzero for normal orientation
	bif.bmiHeader.biSizeImage = 0;
	bif.bmiHeader.biBitCount = 32;//Color depth
	bif.bmiHeader.biPlanes = 1;
	bif.bmiHeader.biCompression = BI_RGB;

	SetDIBitsToDevice(hdc,
		0,
		0,
		width,
		height,
		0,
		0,
		0,
		height,
		image_buf,
		&bif,
		DIB_RGB_COLORS);
}

void Screen::draw_background()
{
	COLORREF rgba = background.get_win_rgb();
	memset(image_buf, rgba, buf_size / 4 * sizeof(rgba));
}

void Screen::update_param(HDC nhdc, int nwidth, int nheight)
{
	hdc = nhdc;
	width = nwidth;
	height = nheight;
		

}

float Screen::get_aspect_ratio()
{
	return (float)height / width;
}

void Screen::draw_line(int x0, int y0, int x1, int y1, Color color)
{


	bool transpose = false;
	if (abs(x1 - x0) < abs(y1 - y0))
	{
		transpose = true;
		std::swap(x0, y0);
		std::swap(x1, y1);
	}

	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	double derror = 0;
	double d = (dx ? dy / (double)dx : 1);
	int y = y0;

	/*int l_bor = (x0 > 0 ? x0 : 0);
	int r_bor = (x1 <= width ? x1 : width);*/

	for (int x = x0; x <= x1; ++x)
	{
		if (transpose)
		{
			if(y >=0 && y <= width && x >=0 && x <= height) 
				set_rgb_pixel(y, x, color);
		}
		else
		{
			if (x >= 0 && x <= width && y >= 0 && y <= height)
				set_rgb_pixel(x, y, color);
		}
		derror += d;
		if (derror >= 0.5)
		{
			derror -= 1.0;
			if (y0 > y1)
			{
				--y;
			}
			else
			{
				++y;
			}
		}
	}
	
}

void Screen::draw_line(float x0, float y0, float x1, float y1, Color color)
{
	draw_line( int(x0), int(y0), int(x1+0.5f), int(y1 + 0.5f), color); 
}

HDC Screen::get_hdc()
{
	return hdc;
}

int Screen::get_width()
{
	return width;
}

int Screen::get_height()
{
	return height;
}

void Screen::create_buf()
{
	buf_size = 4 * width * height;
	image_buf = new BYTE[buf_size];
}

void Screen::create_zbuf()
{
	zbuf_size = width * height;
	z_buf = new float[zbuf_size];
	std::fill_n(z_buf, zbuf_size, std::numeric_limits<float>::max());

}


void Screen::clear_zbuf()
{
	std::fill_n(z_buf, zbuf_size, std::numeric_limits<float>::max());
}

void Screen::set_zbuf_depth(int x, int y, float depth)
{
	z_buf[y * width + x] = depth;
}

float Screen::get_zbuf_detpth(int x, int y)
{
	return z_buf[y * width + x];

}

