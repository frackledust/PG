#pragma once
#include "simpleguidx11.h"
#include "surface.h"
#include "camera.h"
#include "ray.h"
#include "SphereMap.h"
#include "BVH.h"

class Pathtracer : public SimpleGuiDX11
{
public:
	Pathtracer( int width, const int height,
		const float fov_y, const Vector3 view_from, const Vector3 view_at,
		const char * config = "threads=0,verbose=3" );
	~Pathtracer();

	int InitDeviceAndScene( const char * config );

	int ReleaseDeviceAndScene();

	void LoadScene( const std::string file_name );

	Color4f get_pixel( int x, int y, float t = 0.0f ) override;

	int Ui() override;

    void LoadBackground();
private:
	std::vector<Surface *> surfaces_;
	std::vector<Material *> materials_;
    std::unique_ptr<SphereMap> background_;
    std::unique_ptr<BVH> bvh_;

	RTCDevice device_;
	RTCScene scene_;
	Camera camera_;

    //    Vector3 omni_light_position_ = Vector3(100, 0, 130);
    Vector3 omni_light_position_ = Vector3(0, 0, 485);

    bool is_visible(Vector3 hit_point, Vector3 light_point);

    Ray make_secondary_ray(const Vector3 &origin, const Vector3 &dir, float ior);

    Vector3 trace(Ray ray, int depth);

    Vector3 sample_cosine_hemisphere(Normal3f vector3);

    Vector3 sample_hemisphere(Normal3f normal, float &pdf);

    Vector3 sample_cosine_hemisphere(Normal3f normal, float &pdf);
};
