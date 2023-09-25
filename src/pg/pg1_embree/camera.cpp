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

RTCRay Camera::GenerateRay(const float x_i, const float y_i) const
{
	RTCRay ray = RTCRay();

	// TODO fill in ray structure and compute ray direction
	// ray.org_x = ...	
	Vector3 d_c = Vector3(x_i - this->width_ * 0.5f, this->height_ * 0.5f - y_i, this->f_y_ * -1);
	d_c.Normalize();
	Vector3 d_w = this->M_c_w_ * d_c;
	d_w.Normalize();

	ray.org_x = this->view_from_.x; // ray origin
	ray.org_y = this->view_from_.y;
	ray.org_z = this->view_from_.z;
	ray.tnear = FLT_MIN; // start of ray segment

	ray.dir_x = d_w.x; // ray direction
	ray.dir_y = d_w.y;
	ray.dir_z = d_w.z;
	ray.time = 0.0f;

	ray.tfar = FLT_MAX; // end of ray segment (set to hit distance)

	ray.mask = 0; // can be used to mask out some geometries for some rays
	ray.id = 0; // identify a ray inside a callback function
	ray.flags = 0; // reserved

	return ray;
}
