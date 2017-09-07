#pragma once

#include "mat_vec.h"
#define _USE_MATH_DEFINES
#include <math.h>

struct Camera
{
	float field_of_view, aspect_ratio, near_plane, far_plane;
	Vec3f position_, focus_, up_vec_;	

	float move_step;
	Vec3f ort_i, ort_j, ort_k;
	Vec3f world_ort_i, world_ort_j, world_ort_k;
	Vec3f model_ort_i, model_ort_j, model_ort_k;


	Camera(
		float field_of_view = 45.f,
		float aspect_ratio = 3.f / 4.f,
		float near_plane = 1.f,
		float far_plane = 100.f, 
		Vec3f position = Vec3f(-30.f, 30.f, 30.f), //(3.9f, 1.f, -3.f), //(-4.5f, 3.8f, 5.8f),
		Vec3f focus = Vec3f(0, 0, 0),
		Vec3f up_vec = Vec3f(0, 5, 0)

		);
	void up();
	void down();
	void left();
	void right();

	void forward();
	void back();

	void rot_up();
	void rot_down();
	void rot_left();
	void rot_right();

	void field_of_view_up();
	void field_of_view_down();

	void create_ijk_orts();
	void update_param(
		float field_of_view,
		float aspect_ratio,
		float near_plane = 1.f,
		float far_plane = 100.f);

	void prepare_rot_matrix(const Vec3f &axis, float theta);
	void rotate_about_axis(float theta);


};