#include "stdafx.h"
#include "camera.h"

Camera::Camera(const int width, const int height, const float fov_y,
	const Vector3 view_from, const Vector3 view_at)
{
	width_ = width;
	height_ = height;
	fov_y_ = fov_y;

	view_from_ = view_from;
	view_at_ = view_at;

	// TODO compute focal lenght based on the vertical field of view and the camera resolution
	f_y_ = (float)height / (2 * tanf(fov_y / 2));

	// TODO build M_c_w_ matrix	
	// M_c_w_ = Matrix3x3( x_c, y_c, z_c );
	Vector3 z_c = view_from - view_at;
	z_c.Normalize();
	Vector3 x_c = up_.CrossProduct(z_c);
	x_c.Normalize();
	Vector3 y_c = z_c.CrossProduct(x_c);
	y_c.Normalize();

	M_c_w_ = Matrix3x3(x_c, y_c, z_c);
}

Ray Camera::GenerateRay(const float x_i, const float y_i) const
{
	// TODO fill in ray structure and compute ray direction
	Vector3 d_c = Vector3(x_i - this->width_ * 0.5f, this->height_ * 0.5f - y_i, this->f_y_ * -1);
	d_c.Normalize();
	Vector3 d_w = this->M_c_w_ * d_c;
	d_w.Normalize();

	return {this->view_from_, d_w, FLT_MIN};
}
