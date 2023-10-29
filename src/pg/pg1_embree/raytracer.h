#pragma once
#include "simpleguidx11.h"
#include "surface.h"
#include "camera.h"
#include "ray.h"
#include "SphereMap.h"

/*! \class Raytracer
\brief General ray tracer class.

\author Tomáš Fabián
\version 0.1
\date 2018
*/
class Raytracer : public SimpleGuiDX11
{
public:
	Raytracer( const int width, const int height,
		const float fov_y, const Vector3 view_from, const Vector3 view_at,
		const char * config = "threads=0,verbose=3" );
	~Raytracer();

	int InitDeviceAndScene( const char * config );

	int ReleaseDeviceAndScene();

	void LoadScene( const std::string file_name );

	Color4f get_pixel( const int x, const int y, const float t = 0.0f ) override;

	int Ui();

    void LoadBackground();
private:
	std::vector<Surface *> surfaces_;
	std::vector<Material *> materials_;
    std::unique_ptr<SphereMap> background_;

	RTCDevice device_;
	RTCScene scene_;
	Camera camera_;

    bool is_visible(Vector3 hit_point, Vector3 light_point);

    static Ray make_secondary_ray(const Vector3 &origin, const Vector3 &dir);

    Vector3 get_color_phong(Vector3 hit_point, Vector3 omni_light_position, Vector3 normal, Vector3 v, Vector3 l,
                            Vector3 diffuse_color_v, Vector3 specular_color_v, int depth, int max_depth,
                            Material *material);

    Vector3 get_color_glass(Vector3 hit_point, Vector3 omni_light_position, Vector3 normal, Vector3 v, Vector3 l,
                            Vector3 diffuse_color_v, Vector3 specular_color_v, int depth, int max_depth,
                            Material *material,
                            float ior_from);

    Vector3 trace(Ray &ray, const int depth, const int max_depth, float ior_from);
};
