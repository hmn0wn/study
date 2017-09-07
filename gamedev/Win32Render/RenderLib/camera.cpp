#include "stdafx.h"
#include "camera.h"


Camera::Camera(
	float field_of_view,
	float aspect_ratio,
	float near_plane,
	float far_plane,
	Vec3f position,
	Vec3f focus,
	Vec3f up_vec
	
) :
	
	field_of_view(field_of_view),
	aspect_ratio(aspect_ratio),
	near_plane(near_plane),
	far_plane(far_plane),
	position_(position),
	focus_(focus),
	up_vec_(up_vec)
	
{
	
	move_step = 0.3f;

	ort_i = Vec3f(0, 0, 0);
	ort_j = Vec3f(0, 0, 0);
	ort_k = Vec3f(0, 0, 0);

	world_ort_i = Vec3f(1, 0, 0);
	world_ort_j = Vec3f(0, 1, 0);
	world_ort_k = Vec3f(0, 0, 1);
}

void Camera::up()
{
	position_ = position_ + ort_j * move_step;
	focus_ = focus_ + ort_j * move_step;
}

void Camera::down()
{
	position_ = position_ + ort_j * (-move_step);
	focus_ = focus_ + ort_j * (-move_step);
}

void Camera::left()
{
	position_ = position_ + ort_i * (-move_step);
	focus_ = focus_ + ort_i * (-move_step);
}

void Camera::right()
{
	position_ = position_ + ort_i * move_step;
	focus_ = focus_ + ort_i * move_step;
}

void Camera::forward()
{
	position_ = position_ + ort_k * (-move_step);
	focus_ = focus_ + ort_k * (-move_step);
}
void Camera::back()
{
	position_ = position_ + ort_k * move_step;
	focus_ = focus_ + ort_k * move_step;
}

void Camera::rot_up()
{
	position_ = position_ + ort_j * move_step; //spiral
	//prepare_rot_matrix(ort_i, 10.f);
	//rotate_about_axis(10.f);

}

void Camera::rot_down()
{
	position_ = position_ + ort_j * -move_step;
	//prepare_rot_matrix(ort_i, -10.f);
	//rotate_about_axis(-10.f);


}

void Camera::rot_left()
{
	position_ = position_ + ort_i * (-move_step);
	//prepare_rot_matrix(world_ort_j, -10.f);
	//rotate_about_axis(-10.f);

}

void Camera::rot_right()
{
	position_ = position_ + ort_i * move_step;
	//prepare_rot_matrix(world_ort_j, -10.f);
	//rotate_about_axis(10.f);
}


void Camera::field_of_view_up()
{
	field_of_view += 10.f;
}

void Camera::field_of_view_down()
{
	field_of_view -= 10.f;
}

void Camera::create_ijk_orts()
{
	ort_k = (position_ - focus_).normalize();
	ort_i = cross(up_vec_, ort_k).normalize();
	ort_j = cross(ort_k, ort_i).normalize();
}

void Camera::update_param(float field_of_view, float aspect_ratio, float near, float far)
{
	this->field_of_view = field_of_view;
	this->aspect_ratio = aspect_ratio;
	this->near_plane = near;
	this->far_plane = far;
}


