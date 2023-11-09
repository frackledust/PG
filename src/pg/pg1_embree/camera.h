#ifndef CAMERA_H_
#define CAMERA_H_

#include "vector3.h"
#include "matrix3x3.h"
#include "ray.h"

/*! \class Camera
\brief A simple pin-hole camera.

\author Tomáš Fabián
\version 1.0
\date 2018
*/
class Camera
{
public:
	Camera() = default;

	Camera( int width, int height, float fov_y,
		Vector3 view_from, Vector3 view_at );

    void SetTransformationMatrix();

    Vector3 GetViewFrom() const;

    void Rotate(float angle);

    void Move(float distance);

	/* generate primary ray, top-left pixel image coordinates (xi, yi) are in the range <0, 1) x <0, 1) */
	Ray GenerateRay( float xi, float yi ) const;

    std::vector<Ray> GenerateRays(float x_i, float y_i, float focal_distance, int number_of_rays, float aperture) const;

private:
	int width_{ 640 }; // image width (px)
	int height_{ 480 };  // image height (px)
	float fov_y_{ 0.785f }; // vertical field of view (rad)
	
	Vector3 view_from_; // ray origin or eye or O
	Vector3 view_at_; // target T
	Vector3 up_{ Vector3( 0.0f, 0.0f, 1.0f ) }; // up vector

	float f_y_{ 1.0f }; // focal lenght (px)

	Matrix3x3 M_c_w_; // transformation matrix from CS -> WS

};

#endif
