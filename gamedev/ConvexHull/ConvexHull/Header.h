#pragma once

#include <vector>
#include <set>
#include <string>
#include <iterator>
#include <time.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "tgaimage.h"


using namespace std;


#define DEBUG false

#define PI 3.14159265
#define CANVAS_SIZE 768
#define ANGLE_PRECISION 100
TGAImage image(CANVAS_SIZE, CANVAS_SIZE, TGAImage::RGB);
string tga_reader_path = "C:\\Program Files (x86)\\QuickTime\\PictureViewer.exe";

const TGAColor black  = TGAColor(0, 0, 0, 255);
const TGAColor white  = TGAColor(255, 255, 255, 255);
const TGAColor red    = TGAColor(255, 0, 0, 255);
const TGAColor blue   = TGAColor(0, 0, 255, 255);
const TGAColor green  = TGAColor(0, 255, 0, 255);
const TGAColor violet = TGAColor(140, 0, 255, 255);
const TGAColor yellow = TGAColor(255, 255, 0, 255);



struct Point
{
	int x;
	int y;
	int angle;

	Point(int x, int y, int angle);
	bool operator<(const Point &pr) const;
	bool operator!=(const Point &pr) const;
	bool operator==(const Point &pr) const;
	void set_angle(Point const &o);

};


	
TGAColor generate_color();
int		get_angle(Point const &p, Point const &o);
ostream &operator<<(ostream &stream, const Point &p);
bool	operator==(vector<Point> &v_p, multiset<Point> &s_p);
void	print_digit(int px, int py, const int *digit, TGAImage &image, TGAColor color, int line_width);
void	print_num(int x, int y, int number, TGAImage &image, TGAColor color, int line_width);
void	line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void	line(Point const &p0, Point const &p1, TGAImage &image, TGAColor color);
void	print(vector<Point> &v_p);
void	print(multiset<Point> &s_p);
void	print(vector<Point> &v_p, TGAImage &image, TGAColor color, bool with_angle);
void	print(multiset<Point> &s_p, TGAImage &image, TGAColor color, bool with_angle);
void	save_image(TGAImage &image, string name, bool show);
void	print_centroid_segments(Point const &centroid, vector<Point> const &v_p, TGAImage &image, TGAColor color);
void	print_centroid_segments(Point const &centroid, multiset<Point> const &s_p, TGAImage &image, TGAColor color);
int		area2(Point p0, Point p1, Point p2);

Point seg_center(Point const &p0, Point const &p1);
Point segents_cross(Point const &p0, Point const &p1, Point const &p2, Point const &p3);
Point triangle_centroid(Point  a, Point  b, Point  c);

bool horizon_cross(Point p, Point p0, Point p1);
bool in_triangle(Point p, vector<Point> &v_p);
bool not_extreme(Point const &p, multiset<Point> &s_p);

multiset<Point> create_hull_N4(multiset<Point> &s_p);
vector<Point>	remove_dublicates(vector<Point> &v_p);
vector<Point>	create_hull_N3(vector<Point> &v_p);
int				find_left(vector<Point> &v_p);
vector<Point>	create_hull_Jarvis_N2(vector<Point> &v_p);
vector<Point>	generate_vector(int n);
multiset<Point> generate_set(int n);
Point			set_angles(vector<Point> &v_p);
multiset<Point> vec_to_set(vector<Point> &v_p);

void log_to_file(vector<Point> &v_p, vector<Point> &v_hull, string const& name);
void log_to_file(multiset<Point> &s_p, multiset<Point> &s_hull, string const& name);
void stars(TGAImage &image, int n, int width);
vector<Point> get_from_file(string path);


const int minus_ch[] = {
	0, 0, 0,
	0, 0, 0,
	1, 1, 1,
	0, 0, 0,
	0, 0, 0
};

const int zero[] = {
	1, 1, 1,
	1, 0, 1,
	1, 0, 1,
	1, 0, 1,
	1, 1, 1
};
const int one[] = {
	0, 1, 0,
	0, 1, 0,
	0, 1, 0,
	0, 1, 0,
	0, 1, 0
};
const int two[] = {
	1, 1, 1,
	0, 0, 1,
	1, 1, 1,
	1, 0, 0,
	1, 1, 1
};
const int three[] = {
	1, 1, 1,
	0, 0, 1,
	0, 1, 1,
	0, 0, 1,
	1, 1, 1
};
const int four[] = {
	1, 0, 1,
	1, 0, 1,
	1, 1, 1,
	0, 0, 1,
	0, 0, 1
};
const int five[] = {
	1, 1, 1,
	1, 0, 0,
	1, 1, 1,
	0, 0, 1,
	1, 1, 1
};
const int six[] = {
	1, 1, 1,
	1, 0, 0,
	1, 1, 1,
	1, 0, 1,
	1, 1, 1
};
const int seven[] = {
	1, 1, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1
};
const int eight[] = {
	1, 1, 1,
	1, 0, 1,
	1, 1, 1,
	1, 0, 1,
	1, 1, 1
};
const int nine[] = {
	1, 1, 1,
	1, 0, 1,
	1, 1, 1,
	0, 0, 1,
	1, 1, 1
};