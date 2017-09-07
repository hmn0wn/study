#include "stdafx.h"
#include "shader.h"


IShader::~IShader() {}

void IShader::base_geometry(Model & model, Triangle & tr, Camera & camera, Light & light)
{
	tr.face_norm_world = v4f_to_v3f(model_matrix	* embed(tr.norm_model, 0.f)); //!!!MODEL MATRIX MAST BE INV TRANSP

	for (int jvert = 0; jvert < 3; ++jvert)
	{
		tr.v_world[jvert] = v4f_to_v3f(model_matrix	* embed(tr.v_model[jvert]));
		tr.v_norm_world[jvert] = v4f_to_v3f(model_matrix		* embed(tr.v_norm_model[jvert], 0.f));

		tr.v_4d_clip[jvert] = vpvm * embed(tr.v_model[jvert]);

		tr.v_scr[jvert] = v4f_to_v3f(tr.v_4d_clip[jvert]);

	}
}

FlatLambertianShader::FlatLambertianShader()
{
	
}

void FlatLambertianShader::geometry(Model & model, Triangle & tr, Camera & camera, Light & light)
{
	base_geometry(model, tr, camera, light);
}

void FlatLambertianShader::fragment(Model &model, Triangle & tr, Vec3f const & p_4d_clip_bar, Light & light, Color & color)
{
	Vec3f light_to_dir = light.get_pos() - tr.bar_center_world;
	float flat_intensity = std::max(0.f, tr.face_norm_world * light_to_dir.normalize());
	
	color = light.get_color()*flat_intensity;
}


InterpolatedLambertianShader::InterpolatedLambertianShader()
{
}

void InterpolatedLambertianShader::geometry(Model &model, Triangle & tr, Camera &camera, Light &light)
{
	base_geometry(model, tr, camera, light);

	for (int jvert = 0; jvert < 3; ++jvert)
	{
		//varying_intensity[jvert] = std::max(0.f, tr.v_norm_world[jvert] * light_dir);
		varying_intensity[jvert] = std::max(0.f, -1.f * (tr.v_norm_world[jvert] * (tr.bar_center_world - light.get_pos()).normalize()));

	}

}

void InterpolatedLambertianShader::fragment(Model &model, Triangle & tr, Vec3f const &p_4d_clip_bar, Light &light, Color &color)
{
	float intensity = varying_intensity*p_4d_clip_bar;
	color = light.get_color()*intensity;
}


GouraudShader::GouraudShader()
{
}

void GouraudShader::geometry(Model &model, Triangle & tr, Camera &camera, Light &light)
{
	base_geometry(model, tr, camera, light);

	for (int jvert = 0; jvert < 3; ++jvert)
	{

		Vec3f dir_to_light = (light.get_pos() - tr.v_world[jvert]).normalize();
		primary_intensity[jvert] = std::max(0.f, tr.v_norm_world[jvert] * dir_to_light);
		
		Vec3f dir_to_camera = (camera.position_ - tr.v_world[jvert]).normalize();
		Vec3f halfway = (dir_to_light + dir_to_camera).normalize();
		secondary_intensity[jvert] = primary_intensity[jvert] < 1e-4 ? 0.f:
			std::powf(std::max(0.f, tr.v_norm_world[jvert] * halfway), model.get_specular_exp());

	
		 
	}

}

void GouraudShader::fragment(Model &model, Triangle & tr, Vec3f const &p_4d_clip_bar, Light &light, Color &color)
{
	/*Vec4f  v_4d_clip(0,0,0,0);
	for (int jvert = 0; jvert < 3; ++jvert)
	{
		v_4d_clip = v_4d_clip + tr.v_4d_clip[jvert] * p_4d_clip_bar[jvert];
	}*/

	//Vec3f v_world = v4f_to_v3f(vpv.invert_transpose());
	light.set_k(1.f, 0.f, 0.f);
	Color light_color = light.get_point_intensity(tr.bar_center_world);

	primary_color =
		model.get_emission() +
		model.get_diffuse() * light.get_ambient() +
		model.get_diffuse() * light_color * (primary_intensity * p_4d_clip_bar);

	secondary_color =
		model.get_specular() *light_color * (secondary_intensity * p_4d_clip_bar);

	color = (primary_color + secondary_color ).clamp();

}




PhongShader::PhongShader()
{
}

void PhongShader::geometry(Model & model, Triangle & tr, Camera & camera, Light & light)
{
	base_geometry(model, tr, camera, light);

	distance_to_light = (light.get_pos() - tr.bar_center_world).norm();

	for (int jvert = 0; jvert < 3; ++jvert)
	{

		dir_to_light[jvert] = (light.get_pos() - tr.v_world[jvert]).normalize();
		dir_to_camera[jvert] = (camera.position_ - tr.v_world[jvert]).normalize();


	}
}

void PhongShader::fragment(Model & model, Triangle & tr, Vec3f const & p_4d_clip_bar, Light & light, Color & color)
{
	Vec3f dir_to_light_interpolated;
	Vec3f dir_to_camera_imterpolated;
	Vec3f normal_imterpolated;
	Vec3f point_world;

	for (int jvert = 0; jvert < 3; ++jvert)
	{
		dir_to_light_interpolated += dir_to_light[jvert] * p_4d_clip_bar[jvert];
		normal_imterpolated += tr.v_norm_world[jvert] * p_4d_clip_bar[jvert];

	}

	float diffuse_intensity = std::max(0.f, normal_imterpolated * dir_to_light_interpolated);
	if (diffuse_intensity > 1e-4)
	{

		for (int jvert = 0; jvert < 3; ++jvert)
		{
			dir_to_camera_imterpolated += dir_to_camera[jvert] * p_4d_clip_bar[jvert];
			point_world += tr.v_world[jvert] * p_4d_clip_bar[jvert];

		}

		dir_to_light_interpolated.normalize();
		dir_to_camera_imterpolated.normalize();
		normal_imterpolated.normalize();


		Vec3f halfway = (dir_to_light_interpolated + dir_to_camera_imterpolated).normalize();

		float specular_intensity = std::powf(std::max(0.f, normal_imterpolated * halfway), model.get_specular_exp());


		Color light_color = light.get_point_intensity(point_world);
		Color emission_color = model.get_emission();
		Color diffuse_color =
			model.get_diffuse() * (light.get_ambient() + light_color * diffuse_intensity);

		Color specular_color =
			model.get_specular() *light_color * specular_intensity;

		color = (emission_color + diffuse_color + specular_color).clamp();
	}
	else
	{
		Color emission_color = model.get_emission();
		Color diffuse_color =
			model.get_diffuse() * light.get_ambient();

		color = emission_color + diffuse_color;
	}
}




NormalShader::NormalShader()
{
}

void NormalShader::geometry(Model & model, Triangle & tr, Camera & camera, Light & light)
{
	base_geometry(model, tr, camera, light);

}

void NormalShader::fragment(Model & model, Triangle & tr, Vec3f const & p_4d_clip_bar, Light & light, Color & color)
{
	Vec3f normal_imterpolated = Vec3f(1, 1, 1);
	

	for (int jvert = 0; jvert < 3; ++jvert)
	{
		normal_imterpolated += tr.v_norm_world[jvert] * p_4d_clip_bar[jvert];
	}

	normal_imterpolated = normal_imterpolated.normalize();


	color = Color(normal_imterpolated.x, normal_imterpolated.y, normal_imterpolated.z);
}


