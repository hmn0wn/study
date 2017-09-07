#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "model.h"
#include "mat_vec.h"
#include "color.h"

Model::Model()
{
	/*filename_ = "obj/cube.obj";
		emission = color_nm::black;
		diffuse = color_nm::white;
		specular = color_nm::white;
		specular_exponent = 10.f;

	create_model(filename_.c_str());*/    
}

Model::Model(
	const char * filename,
	Color emission,
	Color diffuse,
	Color specular,
	float specular_exponent,
	Vec3f position_world,
	Matrix basis_world):

	filename_(filename),
	emission_(emission),
	diffuse_(diffuse),
	specular_(specular),
	specular_exponent_(specular_exponent),
	position_world_(position_world),
	basis_world_(basis_world)

{
	create_model(filename_.c_str());
}

Model::~Model() {}


void Model::create_model(const char * filename)
{
	std::ifstream in(filename);
	while (in)
	{
		in >> *this;
	}
	create_faces();
	create_vert_normals();
}

void Model::create_faces()
{
	std::vector<short> triangles_face(triangles_num(), -1);
	short counter = 0;
	for (int jtr = 0; jtr < triangles_num(); ++jtr)
	{
		if (triangles_face[jtr] != -1)
			continue;

		for (int ktr = jtr + 1; ktr < triangles_num(); ++ktr)
		{
			if (triangles_face[ktr] != -1)
				continue;

			if
				(
					((get_source_normal(jtr, 0) - get_source_normal(ktr, 0)).norm() < 1e-3) &&
					((get_source_normal(jtr, 1) - get_source_normal(ktr, 1)).norm() < 1e-3) &&
					((get_source_normal(jtr, 2) - get_source_normal(ktr, 2)).norm() < 1e-3)
				)
			{
				triangles_face[ktr] = counter;
			}
		}
		triangles_face[jtr] = counter;
		++counter;

	}

	faces_.resize(counter);
	for (int jtr = 0; jtr < triangles_num(); ++jtr)
	{
		int &count = faces_[triangles_face[jtr]].triangle_count; //does it better then vector<vector..???
		faces_[triangles_face[jtr]].triangles_[count++] = jtr;
	}
}

void Model::create_vert_normals()
{
	vert_norms_.resize(verts_num(),Vec3f(0,0,0));
	for (int kvert = 0; kvert < verts_num(); ++kvert)
	{
		for (int iface = 0; iface < faces_num(); ++iface)
		{
			bool flag = false;

			for (int jtr = 0; jtr < faces_[iface].triangle_count; ++jtr)
			{
				if (flag) continue;

				for (int jvert = 0; jvert < 3; ++jvert)
				{
					if (flag) continue;
					int tr_int = faces_[iface].triangles_[jtr];
					if (kvert == triangles_[tr_int].vert_ind[jvert]) //WTF!!??
					{
						vert_norms_[kvert] = vert_norms_[kvert] + get_source_normal(tr_int, jvert);
						flag = true;
					}

				}
			}
		}
		vert_norms_[kvert].normalize();
	}
}


int Model::verts_num()
{
	return (int)verts_.size();
}

int Model::triangles_num()
{
	return (int)triangles_.size();
}

int Model::faces_num()
{
	return faces_.size();
}

Vec3f Model::vert(int i)
{
	return verts_[i];
}

Vec3f Model::vert(int itr, int jvert)
{
	return verts_[triangles_[itr].vert_ind[jvert]];
}

void Model::rotate_in_model_about_axis(const Vec3f & axis, float theta)
{
	
	

}

void Model::rotate_in_model_about_xyz(float theta)
{
	Matrix m =	get_rot_z_matrix(theta) * 
				get_rot_y_matrix(theta) * 
				get_rot_x_matrix(theta);

	
	for (size_t jvert = 0; jvert < verts_.size(); ++jvert)
	{
		verts_[jvert] = proj(m * embed(verts_[jvert]));
	}
	for (size_t jnorm = 0; jnorm < norms_.size(); ++jnorm)
	{
		norms_[jnorm] = proj(m * embed(norms_[jnorm], 0.f));
	}

	create_vert_normals();

}

void Model::rotate_in_world_about_xyz(float theta)
{
	Matrix m = get_rot_z_matrix(theta) *
		get_rot_y_matrix(theta) *
		get_rot_x_matrix(theta);

	basis_world_ = m * basis_world_;
	position_world_ = proj(m * embed(position_world_));
}

Vec3f Model::get_source_normal(int itr, int jvert) 
{
	int idx = triangles_[itr].normal_ind[jvert];
    return norms_[idx].normalize();
}

Vec3f Model::get_vertex_normal(int itr, int jvert)
{
	return vert_norms_[triangles_[itr].vert_ind[jvert]];
}

Color Model::get_emission()
{
	return emission_;
}

Color Model::get_diffuse()
{
	return diffuse_;
}

Color Model::get_specular()
{
	return specular_;
}

float Model::get_specular_exp()
{
	return specular_exponent_;
}

void Model::specular_exp_up()
{
	specular_exponent_ *= 1.2f;
}

void Model::specular_exp_down()
{
	if (specular_exponent_ > 0.5f)
		specular_exponent_ /= 1.2f;
}

Vec3f Model::get_pos()
{
	return position_world_;
}

Matrix Model::get_basis()
{
	return basis_world_;
}

std::istream & operator >> (std::istream & in, Model & model)
{
	std::string line;
	std::string label;

	if (std::getline(in, line))
	{
		std::stringstream in_line(line);
		in_line >> label;

		if (label == "v")
		{
			Vec3f v;
			in_line >> v.x >> v.y >> v.z;
			model.verts_.push_back(v);
		}
		else
		{
			if (label == "vn")
			{
				Vec3f vn;
				in_line >> vn.x >> vn.y >> vn.z;
				model.norms_.push_back(vn);
			}
			else
			{
				if (label == "f")
				{
					TriangleSource f;
					std::string itmp;
					for (int i = 0; i < 3; ++i)
					{
						std::getline(in_line, itmp, '/');
						f.vert_ind[i] = atoi(itmp.c_str()) - 1; //- 1 to convert 1..n to 0..n-1
						std::getline(in_line, itmp, '/');
						f.texture_ind[i] = atoi(itmp.c_str()) - 1;
						//std::getline(in_line, itmp, ' ');
						in_line >> f.normal_ind[i];
						f.normal_ind[i]--;
						//f.normal_ind[i] = atoi(itmp.c_str()) - 1;//troubles with end of line
					}
					model.triangles_.push_back(f);
				}
			}
		}
	}
	return in;
}
