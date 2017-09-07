#include "stdafx.h"
#include "light.h"

Light::Light(Vec3f position, Vec3f direction, Color color, Color ambient, float kc, float kl, float kq) :

	position_(position),
	direction_(direction),
	color_(color),
	ambient_(ambient),
	kc_(kc),
	kl_(kl),
	kq_(kq)

{}

Vec3f Light::get_pos()

{
	return position_;
}

Vec3f Light::get_dir()
{
	return direction_;
}

Color Light::get_color()
{
	return color_;
}

Color Light::get_ambient()
{
	return ambient_;
}

void Light::move(Vec3f dir)
{
	position_ = position_ + dir;
}

Color Light::get_point_intensity(const Vec3f & obj_position)
{
	float distance = (position_ - obj_position).norm();
	return color_* (1.f / (kc_ + (kl_  + kq_ * distance) * distance));
}

void Light::set_k(float kc, float kl, float kq)
{
	kc_ = kc;
	kl_ = kl;
	kq_ = kq;
}

