#pragma once
#include "mat_vec.h"
#include "model.h"


struct Triangle
{
	//model space
	Vec3f v_model[3];
	Vec3f v_norm_model[3];
	Vec3f norm_model;


	//world space with w division
	Vec3f v_world[3];
	Vec3f v_norm_world[3];
	Vec3f face_norm_world;
	Vec3f bar_center_world;


	//screen space without w division
	Vec4f v_4d_clip[3];

	//screen space with w division
	Vec3f v_scr[3];
	Vec3f ab_scr, ac_scr;
	float abc_area;

	Vec3f bar_center_scr;

	Triangle(Model &model, int itr, bool get_source_vertex_normals_flag = true);

	Vec3f barycentric_cord(Vec3f const &p);


	void calculate_prop();

};