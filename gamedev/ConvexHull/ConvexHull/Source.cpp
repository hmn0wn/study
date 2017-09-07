#include "Header.h"


Point::Point(int x = 0, int y = 0, int angle = 0) : x(x), y(y), angle(angle) {}

bool Point::operator<(const Point &pr) const
{
	return angle < pr.angle;
}

bool Point::operator!=(const Point &pr) const
{
	return x != pr.x || y != pr.y || angle != pr.angle;
}

bool Point::operator==(const Point &pr) const
{
	return x == pr.x && y == pr.y && angle == pr.angle;
}

void Point::set_angle(Point const &o)
{
	angle = get_angle(o, *this);
}



ostream	&operator<<(ostream &stream, const Point &p)
{
	stream << p.x << "\t" << p.y << "\t" << p.angle;
	return stream;
}

bool operator==(vector<Point> &v_p, multiset<Point> &s_p)
{
	if (v_p.size() != s_p.size())
	{
		return false;
	}

	for (auto vit : v_p)
	{
		if (s_p.find(vit) == s_p.end())
		{
			return false;
		}
	}
	return true;
}

void stars(TGAImage &image, int n, int width = 1)
{

	int x = 0;
	int y = 0;
	TGAColor color = white;
	for (int i = 0; i < n; ++i)
	{

		x = rand() % CANVAS_SIZE;
		y = rand() % CANVAS_SIZE;
		//cout << x << " \t " << y << endl;
		color = generate_color();
		for (int it = 0; it < width; ++it)
		for (int jt = 0; jt < width; ++jt)
			image.set(x + jt, y + it, color);
	}
	//cout << endl;
	//save_image(image, "stars.tga");

}

TGAColor generate_color()
{

	return TGAColor(rand() % 256, rand() % 256, rand() % 256, 255);
}

int	get_angle(Point const &o, Point const &p)
{
	if (!(p.x - o.x))
	{
		if (p.y - o.y > 0)
		{
			return 9000;
		}
		if (p.y - o.y < 0)
		{
			return 27000;
		}
	}
	int angle = atan2((p.y - o.y), (p.x - o.x)) * 180 / PI * ANGLE_PRECISION; //  *ANGLE_PRECISION is hack to have more presition
	if (angle < 0)
	{
		angle += 360 * ANGLE_PRECISION;
	}
	return angle;
}


void print_digit(int px, int py, const int *digit, TGAImage &image, TGAColor color, int line_width = 3)
{
	
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			if (digit[i * 3 + j])
			{	
				int x = px + 1 + j * line_width;
				int y = py - 1 - i * line_width;
				for (int li = 0; li < line_width; ++li)
				{
					for (int lj = 0; lj < line_width; ++lj)
					{
						image.set(x + lj, y - li, color);
					}
				}
			}
		}
	}

}

void print_num(int x, int y, int number, TGAImage &image, TGAColor color, int line_width = 3)
{
	image.set(x, y, color);
	
	vector<int> buf;
	const int *digit = NULL;

	if (!number)
	{
		buf.push_back(0);
	}
	if (number < 0)
	{
		number = abs(number);
		digit = minus_ch;
		print_digit(x - 4 * line_width, y, digit, image, color, line_width);
	}
	while (number)
	{
		buf.push_back(number % 10);
		number /= 10;
	}
	
	for (int i = 0; i < buf.size(); ++i)
	{
		switch (buf[buf.size() - 1 - i])
		{
		case 0:
			digit = zero;
			break;
		case 1:
			digit = one;
			break;
		case 2:
			digit = two;
			break;
		case 3:
			digit = three;
			break;
		case 4:
			digit = four;
			break;
		case 5:
			digit = five;
			break;
		case 6:
			digit = six;
			break;
		case 7:
			digit = seven;
			break;
		case 8:
			digit = eight;
			break;
		case 9:
			digit = nine;
			break;
		default:
			break;
		}
		print_digit(x + i * 4 * line_width, y, digit, image, color, line_width);
	}

}

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{

	bool transpose = false;
	if (abs(x1 - x0) < abs(y1 - y0))
	{
		transpose = true;
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1)
	{
		swap(x0, x1);
		swap(y0, y1);
	}

	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	double derror = 0;
	double d = (dx ? dy / (double)dx : 1);
	int y = y0;
	for (int x = x0; x <= x1; ++x)
	{
		if (transpose)
		{
			image.set(y, x, color);
		}
		else
		{
			image.set(x, y, color);
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

void line(Point const &p0, Point const &p1, TGAImage &image, TGAColor color)
{
	line(p0.x, p0.y, p1.x, p1.y, image, color);
}

void print(vector<Point> &v_p)
{
	cout << endl;
	for (int i = 0; i < v_p.size(); ++i)
	{
		cout << i << "\t: " << v_p[i] << endl;
	}
	cout << endl;
}

void print(multiset<Point> &s_p)
{
	cout << endl;
	int i = 0;
	for (multiset<Point>::iterator it = s_p.begin(); it != s_p.end(); ++it)
	{
		cout << i++ << "\t: " << *it << endl;
	}
	cout << endl;
}

void print(vector<Point> &v_p, TGAImage &image, TGAColor color, bool with_angle = false)
{
	if (!v_p.size())
	{
		return;
	}
	
	for (int i = 0; i < v_p.size(); ++i)
	{
		image.set(v_p[i].x, v_p[i].y, color);
		if (i > 0)
		{
			line(v_p[i - 1].x, v_p[i - 1].y, v_p[i].x, v_p[i].y, image, color);
		}
		print_num(v_p[i].x, v_p[i].y, i, image, white, 2);
		if (with_angle)
		{
			print_num(v_p[i].x, v_p[i].y + 6 * 2, v_p[i].angle, image, white, 2);
		}
		//line(0, 0, v_p[i].x, v_p[i].y, image, color);
	}
	line(v_p[0].x, v_p[0].y, v_p[v_p.size() - 1].x, v_p[v_p.size() - 1].y, image, color);

}

void print(multiset<Point> &s_p, TGAImage &image, TGAColor color, bool with_angle = false)
{
	if (!s_p.size())
	{
		return;
	}
	
	multiset<Point>::iterator prev_min_it = s_p.end();
	int i = 0;
	for (multiset<Point>::iterator it = s_p.begin(); it != s_p.end();++it)
	{
		image.set(it->x, it->y, color);
		if (prev_min_it != s_p.end())
		{
			line(prev_min_it->x, prev_min_it->y, it->x, it->y, image, color);
		}
		print_num(it->x, it->y, i++, image, white, 2);
		if (with_angle)
		{
			print_num(it->x, it->y + 6 * 2, it->angle, image, white, 2);
		}

		//line(0, 0, it->.x, it->.y, image, color);
		prev_min_it = it;
	}
	line(s_p.begin()->x, s_p.begin()->y, (--(s_p.end()))->x, (--(s_p.end()))->y, image, color);

}

void save_image(TGAImage &image, string name, bool show = true)
{
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file(name.c_str());
	string path = "cmd /S /C \"\"" + tga_reader_path + "\" \"" + name + "\"\"";
	//cout << path << endl;
	if (show)
	{
		system(path.c_str());
	}
}

void print_centroid_segments(Point const &centroid, vector<Point> const &v_p, TGAImage &image, TGAColor color)
{
	for (vector<Point>::const_iterator it = v_p.begin(); it != v_p.end(); ++it)
	{
		line(centroid, *it, image, color);
	}
}

void print_centroid_segments(Point const &centroid, multiset<Point> const &s_p, TGAImage &image, TGAColor color)
{
	for (multiset<Point>::const_iterator it = s_p.begin(); it != s_p.end(); ++it)
	{
		line(centroid, *it, image, color);
	}
}


int	  area2(Point p0, Point p1, Point p2)	//Double area of p0 - p1p2
{
	return (p1.x - p0.x)*(p2.y - p0.y) - (p2.x - p0.x)*(p1.y - p0.y);
}

Point seg_center(Point const &p0, Point const &p1)
{
	int x = (p1.x - p0.x) / 2 + (p0.x);
	int y = (p1.y - p0.y) / 2 + (p0.y);
	return Point(x, y);
}

Point segents_cross(Point const &p0, Point const &p1, Point const &p2, Point const &p3)
{
	return Point();
}

Point triangle_centroid(Point  a, Point  b, Point  c)
{
	if (a.x > b.x)
	{
		swap(a, b);
	}
	if (b.x > c.x)
	{
		swap(b, c);
	}
	if (a.x > b.x)
	{
		swap(a, b);
	}



	Point a_p_c = seg_center(a, c);
	if (b.x == a_p_c.x)
	{
		swap(b, c);					//if median from b to ac is vertical swap b and c
		a_p_c = seg_center(a, c);
	}
	Point b_p_c = seg_center(b, c);

	//line(b, a_p_c, image, yellow);
	//line(a, b_p_c, image, yellow);

	//cross point fo (a b_p_c) and (b a_p_c)
	double ta = (b_p_c.y - a.y) / double(b_p_c.x - a.x);
	double tb = (a_p_c.y - b.y) / double(a_p_c.x - b.x);

	int xo = ((a.x * ta - a.y) - (b.x * tb - b.y)) / double(ta - tb);
	int yo = (xo - a.x) * ta + a.y;

	return Point(xo, yo);
}


bool horizon_cross(Point p, Point p0, Point p1)
{
	if (p0.y - p1.y == 0)
	{
		return false;
	}

	if (p0.x > p1.x)
	{
		swap(p0, p1);
	}
	double x = (p.y - p0.y)*(p1.x - p0.x) / double(p1.y - p0.y) + p0.x;
	
	if (p0.x <= x && x <= p1.x)
	{
		print_num(x, p.y, 0, image, green, 2);
		return true;
	}

	return false;
}

bool in_triangle(Point p, vector<Point> &v_p)
{
	if ((p.x == 3 && p.y == 171 && v_p[0].x == 561 && v_p[1].x == 7 && v_p[2].x == 2))
		int s = 0;
	vector<int> cross;
	for (int i = 0; i < 3; ++i)
	{
		Point p0 = v_p[i];
		Point p1 = v_p[(i + 1) % 3];

		if (p0.y - p1.y == 0)
		{
			
			if (p0.y == p.y && (p0.x <= p.x && p.x <= p1.x || p1.x <= p.x && p.x <= p0.x))
			{
				cross.push_back(p.x);
			}
			
			{
				continue;
			}
		}

		if (p0.x - p1.x == 0)
		{

			if ((p0.y <= p.y && p.y <= p1.y || p1.y <= p.y && p.y <= p0.y))
			{
				cross.push_back(p0.x);/*
				print(v_p, image, blue);
				print_num(p.x, p.y, 0, image, violet, 3);
				string name = "tmplog\\output_" + string(".tga");
				save_image(image, name);
				image.clear();*/
			}
			
			{
				continue;
			}
		}

		double x = ((p.y - p0.y)*(p1.x - p0.x) / double(p1.y - p0.y) + p0.x);
		if (x == p.x && (p0.y <= p.y && p.y <= p1.y || p1.y <= p.y && p.y <= p0.y))
		{

		}
		if (p0.x <= x && x <= p1.x || p1.x <= x && x <= p0.x)
		{
			cross.push_back(x);
		}

	}

	if (cross.size() >= 2)
	{
		sort(cross.begin(), cross.end());
		if (cross[0] <= p.x && p.x <= cross[cross.size() - 1])
		{
			if (DEBUG)
			{
				print(v_p, image, blue);
				print_num(cross[0], p.y, 0, image, green, 2);
				print_num(cross[1], p.y, 0, image, green, 2);
				print_num(p.x, p.y, 0, image, violet, 3);
				string name = "tmplog\\output_" + string(".tga");
				save_image(image, name);
				image.clear();
			}
			return true;
		}
	}
	return false;
}

bool not_extreme(Point const &p, multiset<Point> &s_p)
{

	int i = 0;
	for (multiset<Point>::iterator it = s_p.begin(); it != s_p.end(); ++it)
	{
		if (*it == p)
		{
			continue;
		}
		for (multiset<Point>::iterator jt = s_p.begin(); jt != s_p.end(); ++jt)
		{
			if (it == jt || *jt == p)
			{
				continue;
			}
			for (multiset<Point>::iterator kt = s_p.begin(); kt != s_p.end(); ++kt)
			{
				if (it == kt || jt == kt || *kt == p)
				{
					continue;
				}
				
				vector<Point> triangle = { *it, *jt, *kt };
			
				if (false && (p.x == 3 && p.y == 171 && it->x == 561 && jt->x == 7 && kt->x == 2))
				{
					print(s_p, image, red);
					print_num(p.x, p.y, 0, image, violet, 3);

					sort(triangle.begin(), triangle.end());
					print(triangle, image, blue);

					print_num(it->x, it->y, 0, image, green, 2);
					print_num(jt->x, jt->y, 0, image, green, 2);
					print_num(kt->x, kt->y, 0, image, green, 2);
					++i;
					
					string name = "tmplog\\output_" + to_string(i) + ".tga";

					save_image(image, name);
					image.clear();
				}

				
				//print(s_p, image, red);
				if (in_triangle(p, triangle))
				{
					if (false && p.x == 3 && p.y == 171)
					{
						print(s_p, image, red);
						print_num(p.x, p.y, 0, image, violet, 3);

						sort(triangle.begin(), triangle.end());
						print(triangle, image, blue);

						print_num(it->x, it->y, 0, image, green, 2);
						print_num(jt->x, jt->y, 0, image, green, 2);
						print_num(kt->x, kt->y, 0, image, green, 2);
						++i;

						string name = "tmplog\\output_" + to_string(i) + ".tga";

						save_image(image, name);
						image.clear();
					}
					return true;
				}
				
			}
		}
	}
	return false;
}


multiset<Point> create_hull_N4(multiset<Point> &s_p)
{
	if (s_p.size() < 4)
	{
		return s_p;
	}

	multiset<Point> tmp(s_p);
	for (multiset<Point>::iterator it = s_p.begin(); it != s_p.end(); ++it)
	{
		if (not_extreme(*it, s_p))
		{
			tmp.erase(*it);
		}
	}
	return tmp;
}

vector<Point> remove_dublicates(vector<Point> &v_p)
{
	vector<Point> tmp;
	tmp.reserve(v_p.size());
	tmp.push_back(v_p[0]);
	for (int i = 1; i < v_p.size(); ++i)
	{	
		if (v_p[i - 1] != v_p[i])
		{
			tmp.push_back(v_p[i]);
		}
	}
	sort(tmp.begin(), tmp.end());
	return tmp;
}

vector<Point> create_hull_N3(vector<Point> &v_p)
{
	if (v_p.size() < 4)
	{
		return v_p;
	}

	vector<Point> tmp;
	int n = v_p.size();
	tmp.reserve(n);
	for (int i = 0; i < n; ++i)
	{
		for (int j = i+1; j < n; ++j)
		{

			int up_zero = 0;
			int zero = 0;
			for (int z = 0; z < n; ++z)
			{
				
				int area = area2(v_p[z], v_p[i], v_p[j]);
				if (area > 0)
				{
					++up_zero;
					//cout << "up_zero:" << endl;
					//cout << v_p[z] << " and [" << v_p[i] << " " << v_p[j] << "]" << endl;
				}
				if (area == 0)
				{
					++zero;
					//cout << "zero:" << endl;
					//cout << v_p[z] << " and [" << v_p[i] << " " << v_p[j] << "]" << endl;
				}

				if (v_p[i].x  == 7 && v_p[j].x == 3 && area < 0)
				{
					cout << "below_zero:" << endl;
					cout << v_p[z] << " and [" << v_p[i] << " " << v_p[j] << "]" << endl;
				}

				vector<Point> triangle = { v_p[z], v_p[i], v_p[j] };

				if (v_p[i].x == 7 && v_p[j].x == 3)
					int h = 0;
				if (false)
				{
					print(v_p, image, red);

					print_num(0, CANVAS_SIZE, n, image, white, 2);
					print_num(0, CANVAS_SIZE - 5 * 3, zero, image, white, 2);
					print_num(0, CANVAS_SIZE - 5 * 6, up_zero, image, white, 2);
					//print(s_p, image, yellow);
					print_num(v_p[z].x, v_p[z].y, 0, image, violet, 3);

					sort(triangle.begin(), triangle.end());
					print(triangle, image, blue);

					//print_num(v_p[z].x, v_p[z].y, 0, image, green, 2);
					print_num(v_p[i].x, v_p[i].y, 0, image, green, 2);
					print_num(v_p[j].x, v_p[j].y, 0, image, green, 2);


					string name = "tmplog\\output_" + to_string(i) + ".tga";

					save_image(image, name);
					image.clear();
				}
			}

			if (zero + up_zero == n || up_zero == 0)
			{
				tmp.push_back(v_p[i]);
				tmp.push_back(v_p[j]);
			}
			up_zero = 0;
			zero = 0;
		}
	}

	return remove_dublicates(tmp);
}

int find_left(vector<Point> &v_p)
{
	int l = 0;
	for (int i = 1; i < v_p.size(); ++i)
	{
		if (v_p[l].x > v_p[i].x)
		{
			l = i;
		}
	}
	return l;
}

vector<Point> create_hull_Jarvis_N2(vector<Point> &v_p)
{
	if (v_p.size() < 4)
	{
		return v_p;
	}

	

	vector<Point> tmp;
	tmp.reserve(v_p.size());
	int L = find_left(v_p);
	tmp.push_back(v_p[L]);
	
	int prev_min_i = L;
	int prev_angle = get_angle(v_p[prev_min_i], Point(v_p[prev_min_i].x, v_p[prev_min_i].y - 1));

	while (true)
	{
		if (true)
		{
			print(v_p, image, red);
			print(tmp, image, violet);
			print_num(v_p[prev_min_i].x, v_p[prev_min_i].y, 0, image, green, 2);
			string name = "tmplog\\output_" + to_string(prev_min_i) + ".tga";
			save_image(image, name, true);
			image.clear();
		}

		int min_d_angle = INT_MAX;
		int cur_min_i = 0;
		int cur_angle = 0;
		int last_i = (prev_min_i == L) ? L : (L + 1) % v_p.size();
		for (int i = (prev_min_i + 1) % v_p.size(); i != last_i; i = (++i) % v_p.size())	//Circle list will be better solution 
		{
			
			int cur_angle = get_angle(v_p[prev_min_i], v_p[i]);
			int cur_d_angle = cur_angle - prev_angle;
			if (cur_d_angle < 0)
			{	
				cur_d_angle += 360 * ANGLE_PRECISION;
			}

			if (cur_d_angle < min_d_angle)
			{
				min_d_angle = cur_d_angle;
				cur_min_i = i;				
			}

		}


		if (cur_min_i != L)
		{
			prev_angle = get_angle(v_p[prev_min_i], v_p[cur_min_i]);
			tmp.push_back(v_p[cur_min_i]);
			prev_min_i = cur_min_i;
		}
		else
		{
			break;
		}
	}
	return tmp;
}

vector<Point> generate_vector(int n)
{
	int upper_bound = CANVAS_SIZE;
	srand(time(NULL));
	vector<Point> v_tmp;
	v_tmp.reserve(n);
	for (int i = 0; i < n; ++i)
	{
		v_tmp.push_back(Point(rand() % upper_bound, rand() % upper_bound, i));
	}
	return v_tmp;
}

multiset<Point> generate_set(int n)
{
	int upper_bound = CANVAS_SIZE;
	multiset<Point> s_tmp;
	for (int i = 0; i < n; ++i)
	{
		s_tmp.insert(Point(rand() % upper_bound, rand() % upper_bound, i));
	}
	return s_tmp;
}


Point set_angles(vector<Point> &v_p)
{
	int i = 0, j = 0, k = 0;
	while (!area2(v_p[i], v_p[j], v_p[k]))
	{
		i = rand() % v_p.size();
		j = rand() % v_p.size();
		k = rand() % v_p.size();
	}
	Point o = triangle_centroid(v_p[i], v_p[j], v_p[k]);

	for (int i = 0; i < v_p.size(); ++i)
	{
		v_p[i].set_angle(o);
	}
	return o;
}

multiset<Point> vec_to_set(vector<Point> &v_p)
{
	multiset<Point> s_p;
	for (int i = 0; i < v_p.size(); ++i)
	{
		s_p.insert(v_p[i]);
	}
	return s_p;
}

void log_to_file(vector<Point> &v_p, vector<Point> &v_hull, string const& name)
{
	ofstream out(name);
	for (Point p : v_p)
	{
		out << p << endl;
	}
	out << -1 << "\t" << -1 << "\t" << -1 << endl;
	for (Point p : v_hull)
	{
		out << p << endl;
	}
	out.close();
}

void log_to_file(multiset<Point> &s_p, multiset<Point> &s_hull, string const& name)
{
	ofstream out(name);
	for (Point p : s_p)
	{
		out << p << endl;
	}
	out << -1 << "\t" << -1 << "\t" << -1 << endl;
	for (Point p : s_hull)
	{
		out << p << endl;
	}
	out.close();
}

vector<Point> get_from_file(string path)
{
	ifstream in(path);
	vector<Point> tmp;
	
	int x = 0, y = 0, a = 0;
	while (true)
	{
		in >> x >> y >> a;
		if (a == -1)
		{
			break;
		}
		tmp.push_back(Point(x, y));

	}
	return tmp;
}

void test_N4_N3(int exp_count = 25, int max_point_count = 10, bool show = false)
{
	vector<Point> v_p, v_hull;
	multiset<Point> s_p, s_hull;
	int count = 3;
	int equal = 0, not_equal = 0;

	//cin >> count;

	for (int i = 0; i < exp_count; ++i)
	{
		time_t 
		//v_p = get_from_file("log\\output_1466819200_40_not_equal_N4.txt");
		//count = v_p.size();

		count = 3 + rand() % (max_point_count - 2);
		v_p = generate_vector(count);


		time_t timer = time(NULL);
		string name = "log\\output_" + to_string(timer) + "_" + to_string(count);
		cout << name << endl;



		Point centroid = set_angles(v_p);
		sort(v_p.begin(), v_p.end());
		//print(v_p);

		//cout << "N3 method" << endl;
		v_hull = create_hull_N3(v_p);
		v_hull = remove_dublicates(v_hull);

		//print_centroid_segments(centroid, v_p, image, yellow);
		print(v_p, image, white);
		//cout << "hull:" << endl;
		print(v_hull, image, red);
		save_image(image, name + "_N3" + ".tga", show);
		image.clear();




		//cout << "N4 method" << endl;
		s_p = vec_to_set(v_p);
		s_hull = create_hull_N4(s_p);

		//print(s_p);
		//print_centroid_segments(centroid, s_p, image, yellow);
		print(s_p, image, white);
		//cout << "hull:" << endl;
		print(s_hull, image, red, false);
		save_image(image, name + "_N4" + ".tga", show);
		image.clear();



		if (v_hull == s_hull)
		{
			cout << "EQUAL" << endl;
			name += "_equal";
			++equal;
		}
		else
		{
			cout << "NOT EQUAL" << endl;
			name += "_not_equal";
			++not_equal;

		}

		log_to_file(s_p, s_hull, name + "_N4" + ".txt");


		log_to_file(v_p, v_hull, name + "_N3" + ".txt");


	}

	cout << "EQUALS     = " << equal << endl;
	cout << "NOT EQUALS = " << not_equal << endl;

}

void test_N3_N2(int exp_count = 25, int max_point_count = 10, bool show = false)
{

	{
		vector<Point> v_p, v_hull_N3, v_hull_N2;
		int count = 3;
		int equal = 0, not_equal = 0;

		//cin >> count;

		for (int i = 0; i < exp_count; ++i)
		{

			//v_p = get_from_file("log\\output_1467131751_5_not_equal_N2.txt");
			//count = v_p.size();

			count = 3 + rand() % (max_point_count - 2);
			v_p = generate_vector(count);


			time_t timer = time(NULL);
			string name = "log\\output_" + to_string(timer) + "_" + to_string(count);
			cout << name << endl;



			Point centroid = set_angles(v_p);
			sort(v_p.begin(), v_p.end());
			//print(v_p);

			//cout << "N3 method" << endl;
			v_hull_N3 = create_hull_N3(v_p);
			v_hull_N3 = remove_dublicates(v_hull_N3);
			sort(v_hull_N3.begin(), v_hull_N3.end());

			//print_centroid_segments(centroid, v_p, image, yellow);
			//print(v_p, image, white);
			//cout << "hull:" << endl;
			print(v_hull_N3, image, red, false);
			save_image(image, name + "_N3" + ".tga", show);
			image.clear();




			//cout << "N4 method" << endl;
			
			v_hull_N2 = create_hull_Jarvis_N2(v_p);
			sort(v_hull_N2.begin(), v_hull_N2.end());

			//print(s_p);
			//print_centroid_segments(centroid, v_p, image, yellow);
			print(v_p, image, white);
			//cout << "hull:" << endl;
			print(v_hull_N2, image, red, false);
			save_image(image, name + "_N2" + ".tga", show);
			image.clear();



			if (v_hull_N3 == v_hull_N2)
			{
				cout << "EQUAL" << endl;
				name += "_equal";
				++equal;
			}
			else
			{
				cout << "NOT EQUAL" << endl;
				name += "_not_equal";
				++not_equal;

			}

			log_to_file(v_p, v_hull_N2, name + "_N2" + ".txt");
			log_to_file(v_p, v_hull_N3, name + "_N3" + ".txt");

		}

		cout << "EQUALS     = " << equal << endl;
		cout << "NOT EQUALS = " << not_equal << endl;

	}
}

int main()
{	
	srand(time(0));
	//TGAImage image(CANVAS_SIZE, CANVAS_SIZE, TGAImage::RGB);
	cout << "Compare N4 and N3 algo:" << endl;
	//test_N4_N3(100, 25, false);
	cout << "Compare N3 and N2 algo:" << endl;
	test_N3_N2(10, 45, true);
	return 0;

}