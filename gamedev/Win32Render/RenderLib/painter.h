#pragma once
#define NOMINMAX

#include <Windows.h>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <limits>
#include <stack>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

#include "util.h"
#include "model.h"
#include "mat_vec.h"
#include "color.h"
#include "camera.h"
#include "shader.h"
#include "screen.h"


class Painter
{
public:

	Painter();
	
	Painter(Camera *camera,
		Model *model,
		Screen *screen,
		IShader *shader,
		Light *light
		);
		
	Painter const &operator=(Painter const& p);
	
	~Painter();

	void draw();
	void draw_to_buffer();

	void create_viewport_matrix();
	bool can_see_triangle(Triangle const &tr);
	void create_projection_matrix();
	void lookat_old();
	void lookat();
	void create_mvpv();
	void rasterizef(Triangle &tr);


	void draw_vector(Vec2f p0, Vec2f p1, Color color = color_nm::blue, bool array_tr = true); //in scr cord
	void draw_vector(Vec3f p0, Vec3f p1, Color color = color_nm::blue, bool array_tr = true); //in world cord

	void draw_axis(Vec3f p0, Vec3f i, Vec3f j, Vec3f k, Color color = color_nm::red);
	void draw_light_point();
	void cam_param_to_stack();
	void print_stack();
	void add_to_print_stack(TextToDraw text);

	void camera_dump();
	void dump();

	void mesh_onoff();
	void face_normals_onoff();
	void vertex_normals_onoff();
	void get_vertex_normals_source_onoff();
	void draw_world_axis_onoff();
	void draw_model_axis_onoff();
	void draw_zbuffer_onoff();
	void draw_normal_buffer_onoff();

	void debug_onoff();
	void draw_light_onoff();
	void rot_in_model_xyz_onoff();
	void rot_in_world_xyz_onoff();

	void rot_step_up();
	void rot_step_down();

	void update_shader(IShader *shader);

private:

	Camera  *camera_;
	Model	*model_;
	Screen	*screen_;
	IShader *shader_;
	Light	*light_;


	bool debug_flag;
	bool draw_face_normals_flag;
	bool draw_vertex_normals_flag;
	bool get_source_vertex_normals_flag;
	bool draw_mesh_flag;

	bool draw_world_axis_flag;
	bool draw_model_axis_flag;

	bool draw_z_buffer;
	bool draw_normal_buffer;//now we dont use normal map


	bool draw_light_flag;

	bool rot_in_model_xyz_flag;
	bool rot_in_world_xyz_flag;

	float rot_step;

	std::string log_file_name;
	std::ofstream log_file;
	bool log_flag;

	std::stack<TextToDraw> stack_to_draw;
};
