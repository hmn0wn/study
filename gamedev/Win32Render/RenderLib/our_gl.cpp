#include "stdafx.h"
#define NOMINMAX
#include <cmath>
#include <limits>
#include "our_gl.h"
#include "color.h"
#include <Windows.h>


Matrix ModelView;
Matrix Viewport;
Matrix Projection;


void setRGBPixel(BYTE * image_buf, Color const& pixel_color)
{
	*(image_buf + 2) = BYTE(pixel_color.R * 255.0);
	*(image_buf + 1) = BYTE(pixel_color.G * 255.0);
	*(image_buf + 0) = BYTE(pixel_color.B * 255.0);
}

IShader::~IShader() {}

void viewport(int x, int y, int w, int h) 
{
	//float d = 255.f;
	float d = 1.f;

    Viewport = Matrix::identity();
    Viewport[0][3] = x+w/2.f;
    Viewport[1][3] = y+h/2.f;
    Viewport[2][3] = d/2.f;
    Viewport[0][0] = w/2.f;
    Viewport[1][1] = h/2.f;
    Viewport[2][2] = d/2.f;
}

void projection(float coeff) {
    Projection = Matrix::identity();
    Projection[3][2] = coeff;
}

void lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye-center).normalize();
    Vec3f x = cross(up,z).normalize();
    Vec3f y = cross(z,x).normalize();
	Matrix Minv = Matrix::identity();
	Matrix Tr = Matrix::identity();
    for (int i=0; i<3; i++) {
        Minv[0][i] = x[i];
        Minv[1][i] = y[i];
        Minv[2][i] = z[i];
        Tr[i][3] = -center[i];
    }
	ModelView = Minv * Tr;
}

Vec3f barycentric(Vec3i A, Vec3i B, Vec3i C, Vec3i P) {
    Vec3i s[2];
    for (int i=2; i--; ) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void triangle(Vec3i *pts, IShader &shader, TGAImage &image) {

    Vec2i bboxmin( std::numeric_limits<int>::max(),  std::numeric_limits<int>::max());
    Vec2i bboxmax(-std::numeric_limits<int>::max(), -std::numeric_limits<int>::max());
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::min(bboxmin[j], pts[i][j]);
            bboxmax[j] = std::max(bboxmax[j], pts[i][j]);
        }
    }
    Vec3i P;
    TGAColor color;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            Vec3f c = barycentric(pts[0], pts[1], pts[2], P);
            P.z = std::max(0, std::min(255, int(pts[0].z*c.x + pts[1].z*c.y + pts[2].z*c.z + .5))); // clamping to 0-255 since it is stored in unsigned char
            if (c.x<0 || c.y<0 || c.z<0 ) continue;
            bool discard = shader.fragment(c, color);
			
            if (!discard) {
                image.set(P.x, P.y, color);
            }
        }
    }
}

void triangle(Vec3i *pts, IShader &shader, BYTE *image_buf, size_t width, size_t heiht)
{
	Vec2i bboxmin(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
	Vec2i bboxmax(-std::numeric_limits<int>::max(), -std::numeric_limits<int>::max());
	for (int i = 0; i<3; i++) 
{
		for (int j = 0; j<2; j++) 
{
			bboxmin[j] = std::min(bboxmin[j], pts[i][j]);
			bboxmax[j] = std::max(bboxmax[j], pts[i][j]);
		}
	}
	Vec3i P;
	Color color;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) 
{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) 
{
			Vec3f c = barycentric(pts[0], pts[1], pts[2], P);
			P.z = std::max(0, std::min(255, int(pts[0].z*c.x + pts[1].z*c.y + pts[2].z*c.z + .5))); // clamping to 0-255 since it is stored in unsigned char
			if (c.x<0 || c.y<0 || c.z<0) continue;
			bool discard = shader.fragment(c, color);
			if (!discard) 
{
			
			int index_in_buf = (int(P.x) + int(P.y) * int(width)) * 4;

			setRGBPixel(&image_buf[index_in_buf], color);
			}
		}
	}
}


