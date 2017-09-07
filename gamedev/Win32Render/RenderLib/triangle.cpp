#include "stdafx.h"
#include "triangle.h"

Triangle::Triangle(Model & model, int itr , bool get_source_vertex_normals_flag)
{
	for (int jvert = 0; jvert < 3; jvert++)
	{
		v_model[jvert] = model.vert(itr, jvert);
		if (get_source_vertex_normals_flag)
		{
			v_norm_model[jvert] = model.get_source_normal(itr, jvert);
		}
		else
		{
			v_norm_model[jvert] = model.get_vertex_normal(itr, jvert);
		}
	}

	Vec3f ab_model = v_model[1] - v_model[0];
	Vec3f ac_model = v_model[2] - v_model[0];
	norm_model = cross(ab_model, ac_model).normalize();

}

Vec3f Triangle::barycentric_cord(Vec3f const &p)
{
	Vec3f tmp;
	Vec3f ap = p - v_scr[0];
	tmp[1] = (ac_scr.y * ap.x - ac_scr.x * ap.y) / abc_area;
	tmp[2] = (-ab_scr.y * ap.x + ab_scr.x * ap.y) / abc_area;
	tmp[0] = 1.0f - tmp[1] - tmp[2];
	return tmp;
}

void Triangle::calculate_prop()
{
	ab_scr = v_scr[1] - v_scr[0];
	ac_scr = v_scr[2] - v_scr[0];
	abc_area = ab_scr.x * ac_scr.y - ab_scr.y * ac_scr.x;
	bar_center_scr = v_scr[0] + (ab_scr + ac_scr) / 3.0f;
	
	Vec3f ab_world = v_world[1] - v_world[0];
	Vec3f ac_world = v_world[2] - v_world[0];
	bar_center_world = v_world[0] + (ab_world + ac_world) / 3.0f;

}
