#pragma once

#include <vector>
#include <string>
#include <sstream>
#define _USE_MATH_DEFINES
#include <math.h>

#include "color.h"
#include "mat_vec.h"

#define MAX_TR_IN_FACE 5000


struct TriangleSource {
	unsigned short vert_ind[3];
	unsigned short texture_ind[3];
	unsigned short normal_ind[3];

};

struct Face {
	Face() : triangle_count(0) {}
	int triangle_count;
	unsigned short triangles_[MAX_TR_IN_FACE];

};

class Model {
private:
	
    std::vector<Vec3f> verts_;
	std::vector<TriangleSource>  triangles_;
	std::vector<Face> faces_;
	std::vector<Vec3f> norms_;
	//std::vector<Vec3f> triangle_norms_;
	std::vector<Vec3f> vert_norms_;

	std::string filename_;
	Color emission_;
	Color diffuse_;
	Color specular_;
	float specular_exponent_;

	Vec3f position_world_;
	Matrix basis_world_;

public:
	Model();
	Model(
		const char *filename,
		Color emission = color_nm::black,
		Color diffuse = color_nm::gold_diffuse,
		Color specular = color_nm::gold_specular,
		float specular_exponent = color_nm::gold_specular_exponent,
		Vec3f position_world = Vec3f(0, 0, 0),
		Matrix basis_world = Matrix::identity());

	~Model();

	void create_model(const char *filename);
	void create_vert_normals();
	void create_faces();

    int verts_num();
    int triangles_num();
	int faces_num();

    Vec3f get_source_normal(int itr, int jvert);
	Vec3f get_vertex_normal(int itr, int jvert);

	Color get_emission();
	Color get_diffuse();
	Color get_specular();
	float get_specular_exp();

	void specular_exp_up();
	void specular_exp_down();
	Vec3f get_pos();
	Matrix get_basis();

    Vec3f vert(int i);
    Vec3f vert(int itr, int jvert);

	void rotate_in_model_about_axis(const Vec3f &axis, float theta);
	void rotate_in_model_about_xyz(float theta);
	void rotate_in_world_about_xyz(float theta);



	friend std::istream& operator >> (std::istream& in, Model& model);
	
};

