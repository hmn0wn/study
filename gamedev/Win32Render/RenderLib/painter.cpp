#include "stdafx.h"
#include "painter.h"


Painter::Painter() {}

Painter::Painter(
	Camera *camera,
	Model *model,
	Screen *screen,
	IShader *shader,
	Light *light
	) :

	camera_(camera),
	model_(model),
	screen_(screen),
	shader_(shader),
	light_(light)
	
{
		log_file_name = "log.txt";
		log_flag = false;
		draw_face_normals_flag = false;
		draw_vertex_normals_flag = false;
		get_source_vertex_normals_flag = true;
		draw_mesh_flag = false;
		draw_world_axis_flag = false;
		draw_model_axis_flag = false;
		draw_z_buffer = false;
		draw_normal_buffer = false;


		debug_flag = false;
		draw_light_flag = false;
		rot_in_model_xyz_flag = false;
		rot_in_world_xyz_flag = false;
		rot_step = 1;
		log_file.open(log_file_name);
}

Painter const & Painter::operator=(Painter const & p)
{
	camera_ = p.camera_;
	model_ = p.model_;
	screen_ = p.screen_;
	shader_ = p.shader_;
	light_ = p.light_;

	log_file.close();
	log_file_name = p.log_file_name;
	log_file.open(log_file_name);

	log_flag = p.log_flag;
	debug_flag = p.debug_flag;
	draw_face_normals_flag = p.draw_face_normals_flag;
	draw_vertex_normals_flag = p.draw_vertex_normals_flag;
	get_source_vertex_normals_flag = p.get_source_vertex_normals_flag;
	draw_mesh_flag = p.draw_mesh_flag;
	draw_world_axis_flag = p.draw_world_axis_flag;
	draw_model_axis_flag = p.draw_model_axis_flag;
	draw_z_buffer = p.draw_z_buffer;
	draw_normal_buffer = p.draw_normal_buffer;
	rot_in_model_xyz_flag = p.rot_in_model_xyz_flag;
	rot_in_world_xyz_flag = p.rot_in_world_xyz_flag;

	draw_light_flag = p.draw_light_flag;

	rot_step = p.rot_step;

	return *this;
}

Painter::~Painter()
{
	log_file.close();
	/*delete camera_;
	delete model_;
	delete screen_;
	delete shader_;
	delete light_;*/
}


void Painter::draw()
{
	if (rot_in_model_xyz_flag)
	{
		model_->rotate_in_model_about_xyz(rot_step);
	}

	if (rot_in_world_xyz_flag)
	{
		model_->rotate_in_world_about_xyz(rot_step);
	}

	screen_->draw_background();
	draw_to_buffer();
	screen_->clear_zbuf();

	if (draw_light_flag)
	{
		draw_light_point();
	}
	screen_->draw_buffer();

	if (debug_flag)
	{
		cam_param_to_stack();
	}
	print_stack();

	add_to_print_stack(TextToDraw(L"fov: ", camera_->field_of_view, 10, screen_->get_height() - 30));
	add_to_print_stack(TextToDraw(L"spec_exp: ", model_->get_specular_exp(), 10, screen_->get_height() - 50));


}

void Painter::draw_to_buffer()
{
	lookat();
	create_viewport_matrix();
	create_projection_matrix();
	create_mvpv();
		

		
	for (int itr = 0; itr<model_->triangles_num(); itr++)
	{
		Triangle tr(*model_, itr, get_source_vertex_normals_flag);
		shader_->geometry(*model_, tr, *camera_, *light_);

		tr.calculate_prop(); 

		if (std::abs(tr.abc_area) < 1e-2) //area of tr = 0
			continue;

			
		if (can_see_triangle(tr))	//backface culling
			continue;
			

		if (debug_flag)
		{
			for (int jvert = 0; jvert<3; jvert++) 
			{
					
				Vec4f vert4_mvp = shader_->pvm * embed(tr.v_model[jvert]);
				Vec4f vert4_mv = shader_->vm *  embed(tr.v_model[jvert]);
					
				Vec3f vect3_mvp = v4f_to_v3f(vert4_mvp);
				Vec3f vect3_mv = v4f_to_v3f(vert4_mv);
					
				add_to_print_stack(TextToDraw(L"model", tr.v_model[jvert], int(tr.v_scr[jvert].x), int(tr.v_scr[jvert].y)));
				add_to_print_stack(TextToDraw(L"mv", vect3_mv));
				add_to_print_stack(TextToDraw(L"mvp", vect3_mvp));
			}
		}

		if (log_flag)
		{
			camera_dump();
			log_file << "tr: " << itr << std::endl;
		}

		rasterizef(tr);

	}

	log_flag = false;

	if (draw_world_axis_flag)
	{
		draw_axis(model_->get_pos(), camera_->world_ort_i, camera_->world_ort_j, camera_->world_ort_k);

	}
	if (draw_model_axis_flag)
	{
		Matrix basis = model_->get_basis();
		draw_axis(model_->get_pos(), v4f_to_v3f(basis.col(0)) , v4f_to_v3f(basis.col(1)), v4f_to_v3f(basis.col(2)), color_nm::blue);

	}
}


void Painter::create_viewport_matrix()
{

	shader_->viewport_matrix = Matrix::identity();
	Matrix &vp = shader_->viewport_matrix;

	vp[0][0] = (float)screen_->get_width() / 2.f;
	vp[0][3] = (float)screen_->get_width() / 2.f;

	vp[1][1] = (float)screen_->get_height() / 2.f;
	vp[1][3] = (float)screen_->get_height() / 2.f;
		
}

bool Painter::can_see_triangle(Triangle const & tr)
{
	Vec3f look_to_tr_bar_center = tr.bar_center_world - camera_->position_;
		
	return tr.face_norm_world * look_to_tr_bar_center > 0.f;
}


void Painter::create_projection_matrix()
{
	float alpha_rad = (camera_->field_of_view * (float)M_PI / 180.0f);
	float e = 1.0f / tanf( alpha_rad / 2);
	float n = camera_->near_plane;
	float f = camera_->far_plane;
	float a = camera_->aspect_ratio;
	float l = -n / e;
	float r = -l;
	float b = a * l;
	float t = -b;

	shader_->projection_matrix = Matrix::identity();
	Matrix &m = shader_->projection_matrix;

	m[0][0] = 2.0f * n / (r - l);
	m[0][2] = (r + l) / (r - l);

	m[1][1] = 2.0f * n / (t - b);
	m[1][2] = (t + b) / (t - b);

	m[2][2] = -(f + n) / (f - n);
	m[2][3] = -2.0f*n*f / (f - n);

	m[3][2] = -1.0f;
	m[3][3] = 0;


}

void Painter::lookat_old()
{
	camera_->create_ijk_orts();

	Matrix rotation_matrix = Matrix::identity();
	Matrix move_matrix = Matrix::identity();
	for (int i = 0; i < 3; ++i)
	{
		rotation_matrix[0][i] = camera_->ort_i[i];
		rotation_matrix[1][i] = camera_->ort_j[i];
		rotation_matrix[2][i] = camera_->ort_k[i];

		move_matrix[i][3] = -camera_->position_[i];
	}
	shader_->view_matrix = rotation_matrix*move_matrix;
	shader_->model_matrix = Matrix::identity();

}

void Painter::lookat() 
{
	camera_->create_ijk_orts();

	shader_->model_matrix = model_->get_basis();
	Vec3f pos = model_->get_pos();
	for (int i = 0; i < 3; ++i)
	{
		shader_->model_matrix[i][3] = pos[i];
	}

	Matrix rotation_matrix = Matrix::identity();
	Matrix move_matrix = Matrix::identity();
	for (int i = 0; i < 3; ++i)
	{
		rotation_matrix[0][i] = camera_->ort_i[i];
		rotation_matrix[1][i] = camera_->ort_j[i];
		rotation_matrix[2][i] = camera_->ort_k[i];

		move_matrix[i][3] = -camera_->position_[i];
	}
	shader_->view_matrix = rotation_matrix*move_matrix;
}
	
void Painter::create_mvpv()
{
	shader_->m_inv_t = shader_->model_matrix.invert_transpose();

	shader_->vm =
		shader_->view_matrix*
		shader_->model_matrix;

	shader_->pvm =
		shader_->projection_matrix*
		shader_->vm;
		
	shader_->pvm_inv_t = shader_->pvm.invert_transpose();

	shader_->vpv =
		shader_->viewport_matrix*
		shader_->projection_matrix*
		shader_->view_matrix;

	shader_->vpvm =
		shader_->viewport_matrix*
		shader_->pvm;
	shader_->vpvm_inv_t = shader_->vpvm.invert_transpose();

}

void Painter::rasterizef(Triangle &tr)
{
		
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

	Vec2f glob_border((float)screen_->get_width(), (float)screen_->get_height());
		
	for (int i = 0; i<3; ++i) 
	{
		for (int j = 0; j<2; j++) 
		{
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], tr.v_scr[i][j]));
			bboxmax[j] = std::min(glob_border[j], std::max(bboxmax[j], tr.v_scr[i][j]));
		}
	}
		
	Vec3f p_scr(0.f, 0.f, camera_->near_plane); 
	
	Color color;
	for (p_scr.x = (float)int(bboxmin.x); p_scr.x <= (float)int(bboxmax.x +0.5f); p_scr.x += 1.f)
	{
		for (p_scr.y = (float)int(bboxmin.y); p_scr.y <= (float)int(bboxmax.y + 0.5f); p_scr.y += 1.f)
		{
			Vec3f p_scr_bar = tr.barycentric_cord(p_scr);			

			//if (p_scr_bar.x + p_scr_bar.y< 1.0f) //point in triangle compatison (alpha + betta + gamma = 1)
			if (p_scr_bar.x < 0.0f ||
				p_scr_bar.y < 0.0f ||
				p_scr_bar.z < 0.0f)
			{
				continue;
			}

			float z_4d_clip =
				1.f /
				(	p_scr_bar[0] / tr.v_4d_clip[0].z  +
					p_scr_bar[1] / tr.v_4d_clip[1].z  +
					p_scr_bar[2] / tr.v_4d_clip[2].z
				);


			float zb = screen_->get_zbuf_detpth(int(p_scr.x), int(p_scr.y));

			if (z_4d_clip > zb) //may be smth wrong with near plane
			{
				continue;
			}
			
			screen_->set_zbuf_depth(int(p_scr.x), int(p_scr.y), z_4d_clip);

			
			Vec3f p_4d_clip_bar(
				p_scr_bar[0] / tr.v_4d_clip[0].w,
				p_scr_bar[1] / tr.v_4d_clip[1].w,
				p_scr_bar[2] / tr.v_4d_clip[2].w);

			float wp = 1.f / (p_4d_clip_bar.x + p_4d_clip_bar.y + p_4d_clip_bar.z);
			p_4d_clip_bar = p_4d_clip_bar * wp;



			if (draw_z_buffer)
			{
				float zb_3d_clip = screen_->get_zbuf_detpth(int(p_scr.x), int(p_scr.y)) / wp;
				float zb_color = (-zb_3d_clip + 1.f) / 2.f;
				color = Color(zb_color, zb_color, zb_color);
			}
			else
			{
				shader_->fragment(*model_, tr, p_4d_clip_bar, *light_, color);
			}

			if (log_flag)
			{
				log_file << p_scr.x << " " << p_scr.y << std::endl;
			}

			screen_->set_rgb_pixel(int(p_scr.x), int(p_scr.y), color);				
		}
	}
	if (draw_mesh_flag)
	{
		Color mesh_color = color_nm::blue;
		screen_->draw_line(tr.v_scr[0].x, tr.v_scr[0].y, tr.v_scr[1].x, tr.v_scr[1].y, mesh_color);
		screen_->draw_line(tr.v_scr[0].x, tr.v_scr[0].y, tr.v_scr[2].x, tr.v_scr[2].y, mesh_color);
		screen_->draw_line(tr.v_scr[2].x, tr.v_scr[2].y, tr.v_scr[1].x, tr.v_scr[1].y, mesh_color);
			
	}
	if (draw_face_normals_flag)
	{
		draw_vector(tr.bar_center_world, tr.bar_center_world + tr.face_norm_world/2.0, color_nm::green);
	}

	if (draw_vertex_normals_flag)
	{
		for (int i = 0; i < 3; ++i)
		{
			draw_vector(tr.v_world[i], tr.v_world[i] + tr.v_norm_world[i] / 2.0, color_nm::green);

		}
	}

}



void Painter::draw_vector(Vec2f p0, Vec2f p1, Color color, bool array_tr)
{
	float angle = 12.0f * (float)M_PI / 180.0f;

	Vec2f p0p1 = (p1 - p0);
	float arrow_len = 0.15f * p0p1.norm();

	Vec2f ort = (p1 - p0).normalize();
	Vec2f ort_t(-ort.y, ort.x);
	Vec2f p1_ = p0p1 - ort*arrow_len + p0;
	Vec2f p2 = p1_ + ort_t * tan(angle)*arrow_len;
	Vec2f p3 = p1_ - ort_t * tan(angle)*arrow_len;


	screen_->draw_line(p0.x, p0.y, p1.x, p1.y, color);
	if (array_tr)
	{
		screen_->draw_line(p1.x, p1.y, p2.x, p2.y, color);
		screen_->draw_line(p1.x, p1.y, p3.x, p3.y, color);
		screen_->draw_line(p3.x, p3.y, p2.x, p2.y, color);
	}

}

void Painter::draw_vector(Vec3f p0, Vec3f p1, Color color, bool array_tr)
{
	Vec2f p0_scr = proj(v4f_to_v3f(shader_->vpv * embed(p0)));
	Vec2f p1_scr = proj(v4f_to_v3f(shader_->vpv * embed(p1)));

	draw_vector(p0_scr, p1_scr, color, array_tr);

}

void Painter::draw_axis(Vec3f p0, Vec3f i, Vec3f j, Vec3f k, Color color)
{
	draw_vector(p0, p0 + i, color);
	draw_vector(p0, p0 + j, color);
	draw_vector(p0, p0 + k, color);

}

void Painter::draw_light_point()
{
	Vec3f & i = camera_->world_ort_i;
	Vec3f & j = camera_->world_ort_j;
	Vec3f & k = camera_->world_ort_k;

	for (float betta = -90.f; betta < 90.f; betta += 18.f)
	{
		for (float alpha = 0.f; alpha < 360.f; alpha += 36.f)
		{
			Vec3f &p = light_->get_pos();
			Vec3f dir = k * cosf(alpha * (float)M_PI / 180.f) +
				(i * sinf(alpha * (float)M_PI / 180.f) +
				j * sinf(betta * (float)M_PI / 180.f));
			draw_vector(p, p + dir.normalize()/5.f, light_->get_color(), false);

		}

	}
}

void Painter::print_stack()
{
	while (!stack_to_draw.empty())
	{
		TextToDraw &it = stack_to_draw.top();
		//print_vec3(it.v, it.x, it.y);
		screen_->print_text(it.x, it.y, (it.label + it.data).c_str());
		stack_to_draw.pop();
	}
}

void Painter::add_to_print_stack(TextToDraw text)
{
	if (text.y == 0)
	{
		text.x = stack_to_draw.top().x;
		text.y = stack_to_draw.top().y + stack_to_draw.top().line_width;

	}
	stack_to_draw.push(text);

}



void Painter::cam_param_to_stack()
{

	add_to_print_stack(TextToDraw(L"cam_ort_i: ", camera_->ort_i, 10, 20));
	add_to_print_stack(TextToDraw(L"cam_ort_j: ", camera_->ort_j));
	add_to_print_stack(TextToDraw(L"cam_ort_k: ", camera_->ort_k));

	add_to_print_stack(TextToDraw(L"cam_pos: ", camera_->position_));
	add_to_print_stack(TextToDraw(L"cam_focus: ", camera_->focus_));


}

void Painter::camera_dump()
{
	log_file << "camera: " << std::endl;
	log_file << std::to_string(camera_->position_.x) << " ";
	log_file << std::to_string(camera_->position_.y) << " ";
	log_file << std::to_string(camera_->position_.z) << " ";
	log_file << std::endl;

}

void Painter::dump()
{
	log_flag = true;
}

void Painter::mesh_onoff()
{
	draw_mesh_flag = (draw_mesh_flag ? false : true);
}

void Painter::face_normals_onoff()
{
	draw_face_normals_flag = (draw_face_normals_flag ? false : true);
}

void Painter::vertex_normals_onoff()
{
	draw_vertex_normals_flag = (draw_vertex_normals_flag ? false : true);

}

void Painter::get_vertex_normals_source_onoff()
{
		
	get_source_vertex_normals_flag = (get_source_vertex_normals_flag ? false : true);

}

void Painter::draw_world_axis_onoff()
{
	draw_world_axis_flag = (draw_world_axis_flag ? false : true);

}

void Painter::draw_model_axis_onoff()
{
	draw_model_axis_flag = (draw_model_axis_flag ? false : true);

}

void Painter::draw_zbuffer_onoff()
{
	draw_z_buffer = (draw_z_buffer ? false : true);
}

void Painter::draw_normal_buffer_onoff()
{
	draw_normal_buffer = (draw_normal_buffer ? false : true);

}

void Painter::debug_onoff()
{
	debug_flag = (debug_flag ? false : true);
}

void Painter::draw_light_onoff()
{
	draw_light_flag = (draw_light_flag ? false : true);

}

void Painter::rot_in_model_xyz_onoff()
{
	rot_in_model_xyz_flag = (rot_in_model_xyz_flag ? false : true);
}

void Painter::rot_in_world_xyz_onoff()
{
	rot_in_world_xyz_flag = (rot_in_world_xyz_flag ? false : true);

}

void Painter::rot_step_up()
{
	rot_step *= 4.f;
}

void Painter::rot_step_down()
{
	if (rot_step > 0.1f)
		rot_step *= 0.5f;
}

void Painter::update_shader(IShader * shader)
{
	delete shader_;
	shader_ = shader;
		
}
