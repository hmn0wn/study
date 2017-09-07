#pragma once
#include "mat_vec.h"
#include "color.h"
#include "model.h"
#include "triangle.h"
#include "light.h"
#include "camera.h"

struct IShader {
	virtual ~IShader() = 0;
	virtual void geometry(Model &model, Triangle & tr, Camera &camera, Light &light) = 0;
	void base_geometry(Model &model, Triangle & tr, Camera &camera, Light &light);

	virtual void fragment(Model &model, Triangle & tr, Vec3f const &p_4d_clip_bar, Light &light, Color &color) = 0;

	Matrix model_matrix;
	Matrix view_matrix;
	Matrix projection_matrix;
	
	Matrix viewport_matrix;

	Matrix m_inv_t;
	Matrix vm;
	Matrix pvm;
	Matrix pvm_inv_t;

	Matrix vpv;
	Matrix vpvm;
	Matrix vpvm_inv_t;

};



struct FlatLambertianShader : public IShader {

	
	FlatLambertianShader();
	virtual void geometry(Model &model, Triangle & tr, Camera &camera, Light &light);
	virtual void fragment(Model &model, Triangle & tr, Vec3f const &p_4d_clip_bar, Light &light, Color &color);

};

struct InterpolatedLambertianShader : public IShader {

	Vec3f varying_intensity;

	InterpolatedLambertianShader();
	virtual void geometry(Model &model, Triangle & tr, Camera &camera, Light &light);
	virtual void fragment(Model &model, Triangle & tr, Vec3f const &p_4d_clip_bar, Light &light, Color &color);
};

struct GouraudShader : public IShader {

	Vec3f varying_intensity;

	Vec3f primary_intensity;
	Vec3f secondary_intensity;

	Color primary_color;
	Color secondary_color;

	GouraudShader();
	virtual void geometry(Model &model, Triangle & tr, Camera &camera, Light &light);
	virtual void fragment(Model &model, Triangle & tr, Vec3f const &p_4d_clip_bar, Light &light, Color &color);

};



struct PhongShader : public IShader {

	float distance_to_light;
	Vec3f dir_to_light[3];
	Vec3f dir_to_camera[3];

	

	PhongShader();
	virtual void geometry(Model &model, Triangle & tr, Camera &camera, Light &light);
	virtual void fragment(Model &model, Triangle & tr, Vec3f const &p_4d_clip_bar, Light &light, Color &color);

};





struct NormalShader : public IShader {


	NormalShader();
	virtual void geometry(Model &model, Triangle & tr, Camera &camera, Light &light);
	virtual void fragment(Model &model, Triangle & tr, Vec3f const &p_4d_clip_bar, Light &light, Color &color);

};

