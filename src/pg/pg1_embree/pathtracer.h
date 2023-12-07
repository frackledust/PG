#pragma once
#include "simpleguidx11.h"
#include "surface.h"
#include "camera.h"
#include "ray.h"
#include "SphereMap.h"
#include "BVH.h"
#include "TriangleLight.h"

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
    std::vector<std::shared_ptr<TriangleLight>> lights_;

    float * buffer_data;
    int * buffer_count;

	RTCDevice device_;
	RTCScene scene_;
	Camera camera_;

    bool is_visible(Vector3 hit_point, Vector3 light_point);

    Ray make_secondary_ray(const Vector3 &origin, const Vector3 &dir, float ior);

    Vector3 trace(Ray &ray, int depth);

    static Vector3 sample_hemisphere(Normal3f normal, float &pdf);

    static Vector3 sample_cosine_hemisphere(Normal3f normal, float &pdf);

    static float arvo_integrate_modified_phong(Vector3 Normal, Vector3 omega_i, int n);

    Vector3 get_color_lambert(Vector3 diffuse_color, Normal3f normal, Vector3 hit_point, int depth);

    Vector3 get_phong_simple(Normal3f normal, Vector3 omega_o, Vector3 hit_point, Vector3 diffuse_color,
                             Vector3 specular_color, float shininess, int depth);

    Vector3 get_phong_LW(Vector3 normal, Vector3 omega_o, Vector3 hit_point, Vector3 diffuse_color,
                         Vector3 specular_color, float shininess, int depth);

    Vector3 get_phong_arvo(Vector3 normal, Vector3 omega_o, Vector3 hit_point, Vector3 diffuse_color,
                           Vector3 specular_color, float shininess, int depth);

    Vector3 sample_cosine_lobe(Vector3 omega_r, float gamma, float &pdf);

    static void fresnel_reflectance(Vector3 diffuse, Vector3 specular, float cos_theta, Vector3 &F, Vector3 &Rd);

    Vector3 get_direct_light_color(Vector3 hit_point, Vector3 normal, Vector3 diffuse_color);

    Vector3 add_color_nne(Vector3 L_indirect, Vector3 f_r, Vector3 normal, Vector3 hit_point,
                          bool has_hit_light, float russian_roulette);
};
