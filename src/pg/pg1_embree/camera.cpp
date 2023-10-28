#include "stdafx.h"
#include "camera.h"
#include "material.h"

Camera::Camera(const int width, const int height, const float fov_y,
	const Vector3 view_from, const Vector3 view_at)
{
	width_ = width;
	height_ = height;
	fov_y_ = fov_y;

	view_from_ = view_from;
	view_at_ = view_at;

	f_y_ = (float)height / (2 * tanf(fov_y / 2));
    SetTransformationMatrix();
}

void Camera::SetTransformationMatrix() {
    Vector3 z_c = view_from_ - view_at_;
    z_c.Normalize();
    Vector3 x_c = up_.CrossProduct(z_c);
    x_c.Normalize();
    Vector3 y_c = z_c.CrossProduct(x_c);
    y_c.Normalize();

    M_c_w_ = Matrix3x3(x_c, y_c, z_c);
}

Ray Camera::GenerateRay(const float x_i, const float y_i) const
{
	Vector3 d_c = Vector3(x_i - this->width_ * 0.5f, this->height_ * 0.5f - y_i, this->f_y_ * -1);
	d_c.Normalize();
	Vector3 d_w = this->M_c_w_ * d_c;
	d_w.Normalize();

    Ray r = Ray(this->view_from_, d_w, FLT_MIN, IOR_AIR);
	return r;
}

void Camera::Rotate(const float angle) {
    // rotate camera around its target (view_at_) by angle
    auto translated = view_from_ - view_at_;

    //perform rotation
    translated.x = translated.x * cos(angle) - translated.y * sin(angle);
    translated.y = translated.x * sin(angle) + translated.y * cos(angle);

    //translate back
    translated.x += view_at_.x;
    translated.y += view_at_.y;
    translated.z = view_from_.z;

    view_from_ = translated;
    SetTransformationMatrix();
}

Vector3 Camera::GetViewFrom() const {
    return view_from_;
}

void Camera::Move(const float distance) {
    // move camera by distance along its view direction
    auto translated = view_from_ - view_at_;
    translated.Normalize();
    translated *= distance;
    view_from_ += translated;
    SetTransformationMatrix();
}
