#pragma once
#include "stdafx.h"
#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>


struct Vec2f {
	Vec2f();
	Vec2f(float X, float Y);
	float& operator[](const size_t i);
    const float& operator[](const size_t i) const;
	float norm();
	Vec2f & normalize(float l = 1);
    
	float x,y;
};


struct Vec3f {
	Vec3f() ;
	Vec3f(float X, float Y, float Z);
	float& operator[](const size_t i);
	const float& operator[](const size_t i) const;
	float norm();
	float norm2();
    Vec3f & normalize(float l = 1.f);

	float x,y,z;
};


struct Vec4f {
	Vec4f();
	Vec4f(float X, float Y, float Z, float W);
	
	float& operator[](const size_t i);
	const float& operator[](const size_t i) const;
	float norm();
	Vec4f & normalize(float l = 1.f);
	float x, y, z, w;
};



float operator*(const Vec2f& lhs, const Vec2f& rhs);
float operator*(const Vec3f& lhs, const Vec3f& rhs);
float operator*(const Vec4f& lhs, const Vec4f& rhs);

Vec2f operator+(Vec2f lhs, const Vec2f& rhs);
Vec3f operator+(Vec3f lhs, const Vec3f& rhs);
void operator+=(Vec3f &lhs, const Vec3f& rhs);

Vec4f operator+(Vec4f lhs, const Vec4f& rhs);
Vec2f operator-(Vec2f lhs, const Vec2f& rhs);
Vec3f operator-(Vec3f lhs, const Vec3f& rhs);
Vec4f operator-(Vec4f lhs, const Vec4f& rhs);
Vec2f operator*(Vec2f lhs,  float rhs);
Vec3f operator*(Vec3f lhs, float rhs);
Vec4f operator*(Vec4f lhs, float rhs);
Vec2f operator/(Vec2f lhs, float rhs);
Vec3f operator/(Vec3f lhs, float rhs);
Vec4f operator/(Vec4f lhs, float rhs);

Vec3f embed(const Vec2f &v, float fill=1.0);
Vec4f embed(const Vec3f &v, float fill = 1.0);
Vec3f proj(const Vec4f &v);
Vec2f proj(const Vec3f &v);

Vec3f cross(Vec3f v1, Vec3f v2);

std::ostream& operator<<(std::ostream& out, Vec2f& v);
std::ostream& operator<<(std::ostream& out, Vec3f& v);
std::ostream& operator<<(std::ostream& out, Vec4f& v);


class Matrix {
    Vec4f rows[4];
public:
	Matrix();

    Vec4f& operator[] (const size_t idx);
    const Vec4f& operator[] (const size_t idx) const;
    Vec4f col(const size_t idx) const;
    void set_col(size_t idx, Vec4f v);
    static Matrix identity();

	float det() const;
    float cofactor(size_t row, size_t col) const;
    Matrix adjugate() const;
	Matrix invert() const;
    Matrix invert_transpose() const;
	Matrix transpose() const;
};


Vec4f operator*(const Matrix& lhs, const Vec4f& rhs);
Matrix operator*(const Matrix& lhs, const Matrix& rhs);
Matrix operator+(const Matrix& lhs, const Matrix& rhs);
Matrix operator*(const Matrix& lhs, float rhs);


Matrix operator/(Matrix lhs, const float& rhs);
std::ostream& operator<<(std::ostream& out, Matrix& m);

Vec3f v4f_to_v3f(const Vec4f v4f);

Matrix get_rot_y_matrix(float theta);
Matrix get_rot_x_matrix(float theta);
Matrix get_rot_z_matrix(float theta);


Matrix get_rot_matrix(const Vec3f &axis, float theta);
