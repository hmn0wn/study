#include "stdafx.h"
#include "mat_vec.h"



Vec2f::Vec2f() : x(float()), y(float()) {}
Vec2f::Vec2f(float X, float Y) : x(X), y(Y) {}

float& Vec2f::operator[](const size_t i)
{
	assert(i<2); return i <= 0 ? x : y;
}

const float& Vec2f::operator[](const size_t i) const
{
	assert(i<2); return i <= 0 ? x : y;
}

float Vec2f::norm()
{
	return std::sqrt(x*x + y*y);
}

Vec2f & Vec2f::normalize(float l)
{
	*this = (*this)*(l / norm());
	return *this;
}



Vec3f::Vec3f() : x(float()), y(float()), z(float()) {}
Vec3f::Vec3f(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

float& Vec3f::operator[](const size_t i)
{
	assert(i<3); return i <= 0 ? x : (1 == i ? y : z);
}

const float& Vec3f::operator[](const size_t i) const
{
	assert(i<3); return i <= 0 ? x : (1 == i ? y : z);
}

float Vec3f::norm()
{
	return std::sqrt(x*x + y*y + z*z);
}

float Vec3f::norm2()
{
	return (x + y + z);
}

Vec3f & Vec3f::normalize(float l)
{
	*this = (*this)*(l / norm()); return *this;
}



Vec4f::Vec4f() : x(float()), y(float()), z(float()), w(float()) {}
Vec4f::Vec4f(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}

float& Vec4f::operator[](const size_t i)
{
	assert(i<4); return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w));
}
const float& Vec4f::operator[](const size_t i) const
{
	assert(i<4); return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w));
}

float Vec4f::norm()
{
	return std::sqrt(x*x + y*y + z*z + w*w);
}

Vec4f & Vec4f::normalize(float l)
{
	*this = (*this)*(l / norm()); return *this;
}



float operator*(const Vec2f& lhs, const Vec2f& rhs)
{
	float ret = float();
	for (size_t i = 2; i--; ret += lhs[i] * rhs[i]);
	return ret;
}

float operator*(const Vec3f& lhs, const Vec3f& rhs)
{
	float ret = float();
	for (size_t i = 3; i--; ret += lhs[i] * rhs[i]);
	return ret;
}

float operator*(const Vec4f& lhs, const Vec4f& rhs)
{
	float ret = float();
	for (size_t i = 4; i--; ret += lhs[i] * rhs[i]);
	return ret;
}



Vec2f operator+(Vec2f lhs, const Vec2f& rhs)
{
	for (size_t i = 2; i--; lhs[i] += rhs[i]);
	return lhs;
}

Vec3f operator+(Vec3f lhs, const Vec3f& rhs)
{
	for (size_t i = 3; i--; lhs[i] += rhs[i]);
	return lhs;
}

void operator+=(Vec3f & lhs, const Vec3f & rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
}

Vec4f operator+(Vec4f lhs, const Vec4f& rhs)
{
	for (size_t i = 4; i--; lhs[i] += rhs[i]);
	return lhs;
}


Vec2f operator-(Vec2f lhs, const Vec2f& rhs)
{
	for (size_t i = 2; i--; lhs[i] -= rhs[i]);
	return lhs;
}

Vec3f operator-(Vec3f lhs, const Vec3f& rhs)
{
	for (size_t i = 3; i--; lhs[i] -= rhs[i]);
	return lhs;
}

Vec4f operator-(Vec4f lhs, const Vec4f& rhs)
{
	for (size_t i = 4; i--; lhs[i] -= rhs[i]);
	return lhs;
}

Vec2f operator*(Vec2f lhs, float rhs)
{
	for (size_t i = 2; i--; lhs[i] *= rhs);
	return lhs;
}

Vec3f operator*(Vec3f lhs, float rhs)
{
	for (size_t i = 3; i--; lhs[i] *= rhs);
	return lhs;
}

Vec4f operator*(Vec4f lhs, float rhs)
{
	for (size_t i = 4; i--; lhs[i] *= rhs);
	return lhs;
}

Vec2f operator/(Vec2f lhs, float rhs)
{
	for (size_t i = 2; i--; lhs[i] /= rhs);
	return lhs;
}

Vec3f operator/(Vec3f lhs, float rhs)
{
	for (size_t i = 3; i--; lhs[i] /= rhs);
	return lhs;
}

Vec4f operator/(Vec4f lhs, float rhs)
{
	for (size_t i = 4; i--; lhs[i] /= rhs);
	return lhs;
}

Vec3f embed(const Vec2f &v, float fill)
{
	Vec3f ret;
	for (size_t i = 3; i--; ret[i] = (i<2 ? v[i] : fill));
	return ret;
}

Vec4f embed(const Vec3f &v, float fill)
{
	Vec4f ret;
	for (size_t i = 4; i--; ret[i] = (i<3 ? v[i] : fill));
	return ret;
}

Vec3f proj(const Vec4f &v)
{
	Vec3f ret;
	for (size_t i = 3; i--; ret[i] = v[i]);
	return ret;
}

Vec2f proj(const Vec3f &v)
{
	Vec2f ret;
	for (size_t i = 2; i--; ret[i] = v[i]);
	return ret;
}


Vec3f cross(Vec3f v1, Vec3f v2)
{
	return Vec3f(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}


std::ostream& operator<<(std::ostream& out, Vec2f& v)
{
	for (unsigned int i = 0; i<2; ++i) {
		out << v[i] << " ";
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, Vec3f& v)
{
	for (unsigned int i = 0; i<3; ++i) {
		out << v[i] << " ";
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, Vec4f& v)
{
	for (unsigned int i = 0; i<4; ++i) 
	{
		out << v[i] << " ";
	}
	return out;
}




Matrix::Matrix() {}

Vec4f& Matrix::operator[] (const size_t idx)
{
	assert(idx<4);
	return rows[idx];
}

const Vec4f& Matrix::operator[] (const size_t idx) const
{
	assert(idx<4);
	return rows[idx];
}

Vec4f Matrix::col(const size_t idx) const
{
	assert(idx<4);
	Vec4f ret;
	for (size_t i = 4; i--; ret[i] = rows[i][idx]);
	return ret;
}

void Matrix::set_col(size_t idx, Vec4f v)
{
	assert(idx<4);
	for (size_t i = 4; i--; rows[i][idx] = v[i]);
}

Matrix Matrix::identity() {
	Matrix ret;
	for (size_t i = 4; i--; )
		for (size_t j = 4; j--; ret[i][j] = (i == j));
	return ret;
}

float Matrix::det() const
{
	float det_ = 0.f;
	int j = 0;
	for (int i = 0; i < 4; ++i)
	{
		det_ += rows[i][j] * cofactor(i, j);
	}
	return det_;
}



float Matrix::cofactor(size_t row, size_t col) const
{
	unsigned short i[3], j[3];
	int k = 0;
	for (int i_ = 0; i_ < 4; ++i_)
		if (i_ != row)
			i[k++] = i_;

	k = 0;
	for (int j_ = 0; j_ < 4; ++j_)
		if (j_ != col)			
			j[k++] = j_;

	Vec4f const *a = rows;
	float det_ =
		a[i[0]][j[0]] * (a[i[1]][j[1]] * a[i[2]][j[2]] - a[i[1]][j[2]] * a[i[2]][j[1]]) +
		a[i[0]][j[1]] * (a[i[1]][j[0]] * a[i[2]][j[2]] - a[i[1]][j[2]] * a[i[2]][j[0]]) -
		a[i[0]][j[2]] * (a[i[1]][j[0]] * a[i[2]][j[1]] - a[i[1]][j[1]] * a[i[2]][j[0]]);

	return det_*((row + col) % 2 ? -1 : 1);
}

Matrix Matrix::adjugate() const
{
	Matrix ret;
	for (size_t i = 4; i--; )
		for (size_t j = 4; j--; ret[i][j] = cofactor(i, j));
	return ret;
}

Matrix Matrix::invert() const
{
	Matrix ret = adjugate();
	float tmp = ret[0] * rows[0]; //det
	return ret / tmp;
}

Matrix Matrix::invert_transpose() const
{
	return (*this).invert().transpose();
}

Matrix Matrix::transpose() const
{
	Matrix ret = identity();
	for (int i = 0; i < 4; ++i)
	{
		ret[i] = col(i);
	}
	return ret;
}




Vec4f operator*(const Matrix& lhs, const Vec4f& rhs)
{
	Vec4f ret;
	for (size_t i = 4; i--; ret[i] = lhs[i] * rhs);
	return ret;
}


Matrix operator*(const Matrix& lhs, const Matrix& rhs)
{
	Matrix result;
	for (size_t i = 4; i--; )
		for (size_t j = 4; j--; result[i][j] = lhs[i] * rhs.col(j));
	return result;
}

Matrix operator+(const Matrix & lhs, const Matrix & rhs)
{
	Matrix result;
	for (size_t i = 4; i--; )
		 result[i] = lhs[i] + rhs[i];
	return result;
}

Matrix operator*(const Matrix & lhs, float rhs)
{
	Matrix result;
	for (size_t i = 4; i--; )
		result[i] = lhs[i] * rhs;
	return result;
}


Matrix operator/(Matrix lhs, const float& rhs)
{
	for (size_t i = 4; i--; lhs[i] = lhs[i] / rhs);
	return lhs;
}


std::ostream& operator<<(std::ostream& out, Matrix& m) {
	for (size_t i = 0; i<4; ++i) out << m[i] << std::endl;
	return out;
}



Vec3f v4f_to_v3f(const Vec4f v4f)
{
	if(fabs(v4f.w) < 1e-3)
		return proj(v4f);
	else
		return proj(v4f / v4f.w);
}

Matrix get_rot_y_matrix(float theta)
{
	Matrix m = Matrix::identity();
	float c = cosf(theta * (float)M_PI / 180.f);
	float s = sinf(theta * (float)M_PI / 180.f);

	m[0][0] = c;
	m[0][1] = -s;
	
	m[1][0] = s;
	m[1][1] = c;

	return m;
}

Matrix get_rot_x_matrix(float theta)
{
	Matrix m = Matrix::identity();
	float c = cosf(theta * (float)M_PI / 180.f);
	float s = sinf(theta * (float)M_PI / 180.f);

	m[1][1] = c;
	m[1][2] = -s;

	m[2][1] = s;
	m[2][2] = c;

	return m;
}

Matrix get_rot_z_matrix(float theta)
{
	Matrix m = Matrix::identity();
	float c = cosf(theta * (float)M_PI / 180.f);
	float s = sinf(theta * (float)M_PI / 180.f);

	m[0][0] = c;
	m[0][2] = s;

	m[2][0] = -s;
	m[2][2] = c;

	return m;
}

Matrix get_rot_matrix(const Vec3f & axis, float theta)
{
	float c = cosf(theta * (float)M_PI / 180.f);
	float s = sinf(theta * (float)M_PI / 180.f);

	Matrix m1 = Matrix::identity();
	m1[0][1] = -axis.z;
	m1[0][2] = axis.y;
	m1[1][2] = -axis.x;

	m1[1][0] = axis.z;
	m1[2][0] = -axis.y;
	m1[2][1] = axis.x;

	Matrix m2 = Matrix::identity();
	m1[0][0] = axis.x * axis.x;
	m1[1][1] = axis.y * axis.y;
	m1[2][2] = axis.z * axis.z;

	m1[0][1] = axis.x * axis.y;
	m1[0][2] = axis.x * axis.z;
	m1[1][2] = axis.y * axis.z;

	m1[1][0] = axis.x * axis.y;
	m1[2][0] = axis.x * axis.z;
	m1[2][1] = axis.y * axis.z;

	return Matrix::identity() * c + m1 * s + m2 * (1 - c);
}

