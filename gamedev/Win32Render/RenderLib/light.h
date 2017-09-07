#pragma once

#include "mat_vec.h"
#include "color.h"

struct Light {

	Light(
		Vec3f position = Vec3f(3, 3, 3),
		Vec3f direction = Vec3f(5, -10, -15),
		Color color = color_nm::white,
		Color ambient = color_nm::gold_ambient,//too_dark_grey*(3.f),
		float kc = 1.f,
		float kl = 0.002f,
		float kq = 0.005f);
	
	Vec3f get_pos();	
	Vec3f get_dir();
	Color get_color();
	Color get_ambient();
	void move(Vec3f dir);
	Color get_point_intensity(const Vec3f &obj_position);

	void set_k(float kc = 1.f,
		float kl = 0.002f,
		float kq = 0.005f);

private:
	Vec3f position_;
	Vec3f direction_;
	Color color_;
	Color ambient_;

	float kc_, kl_, kq_;
};