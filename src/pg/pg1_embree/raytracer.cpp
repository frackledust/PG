#include "stdafx.h"
#include "raytracer.h"
#include "objloader.h"
#include "tutorials.h"
#include "ray.h"
#include "SphereMap.h"
#include "BVH.h"
#include "mymath.h"

Raytracer::Raytracer(const int width, const int height,
	const float fov_y, const Vector3 view_from, const Vector3 view_at,
	const char* config) : SimpleGuiDX11(width, height)
{
	InitDeviceAndScene(config);

	camera_ = Camera(width, height, fov_y, view_from, view_at);
}

Raytracer::~Raytracer()
{
	ReleaseDeviceAndScene();
}

int Raytracer::InitDeviceAndScene(const char* config)
{
	device_ = rtcNewDevice(config);
	error_handler(nullptr, rtcGetDeviceError(device_), "Unable to create a new device.\n");
	rtcSetDeviceErrorFunction(device_, error_handler, nullptr);

	ssize_t triangle_supported = rtcGetDeviceProperty(device_, RTC_DEVICE_PROPERTY_TRIANGLE_GEOMETRY_SUPPORTED);

	// create a new scene bound to the specified device
	scene_ = rtcNewScene(device_);

	return S_OK;
}

int Raytracer::ReleaseDeviceAndScene()
{
	rtcReleaseScene(scene_);
	rtcReleaseDevice(device_);

	return S_OK;
}

void Raytracer::LoadScene(const std::string file_name)
{
	const int no_surfaces = LoadOBJ(file_name.c_str(), surfaces_, materials_);

    std::vector<std::shared_ptr<BVHTriangle>> bvh_triangles;

	// surfaces loop
	for (auto surface : surfaces_)
	{
		RTCGeometry mesh = rtcNewGeometry(device_, RTC_GEOMETRY_TYPE_TRIANGLE);

		Vertex3f* vertices = (Vertex3f*)rtcSetNewGeometryBuffer(
			mesh, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
			sizeof(Vertex3f), 3 * surface->no_triangles());

		Triangle3ui* triangles = (Triangle3ui*)rtcSetNewGeometryBuffer(
			mesh, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,
			sizeof(Triangle3ui), surface->no_triangles());

		rtcSetGeometryUserData(mesh, (void*)(surface->get_material()));

		rtcSetGeometryVertexAttributeCount(mesh, 2);

		Normal3f* normals = (Normal3f*)rtcSetNewGeometryBuffer(
			mesh, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3,
			sizeof(Normal3f), 3 * surface->no_triangles());

		Coord2f* tex_coords = (Coord2f*)rtcSetNewGeometryBuffer(
			mesh, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, RTC_FORMAT_FLOAT2,
			sizeof(Coord2f), 3 * surface->no_triangles());

        unsigned int geom_id = rtcAttachGeometry(scene_, mesh);

		// triangles loop
		for (int i = 0, k = 0; i < surface->no_triangles(); ++i)
		{
			Triangle& triangle = surface->get_triangle(i);

			// vertices loop
			for (int j = 0; j < 3; ++j, ++k)
			{
				const Vertex& vertex = triangle.vertex(j);

				vertices[k].x = vertex.position.x;
				vertices[k].y = vertex.position.y;
				vertices[k].z = vertex.position.z;

				normals[k].x = vertex.normal.x;
				normals[k].y = vertex.normal.y;
				normals[k].z = vertex.normal.z;

				tex_coords[k].u = vertex.texture_coords[0].u;
				tex_coords[k].v = vertex.texture_coords[0].v;
			} // end of vertices loop

			triangles[i].v0 = k - 3;
			triangles[i].v1 = k - 2;
			triangles[i].v2 = k - 1;

            //TODO: create BVH
            std::shared_ptr<BVHTriangle> bvh_triangle = std::make_shared<BVHTriangle>(triangle.vertex(0),
                                                                                      triangle.vertex(1),
                                                                                      triangle.vertex(2));
            bvh_triangle->geom_id = geom_id;
            bvh_triangle->material = surface->get_material();
            bvh_triangle->calculate_bbox();
            bvh_triangles.push_back(bvh_triangle);
		} // end of triangles loop

		rtcCommitGeometry(mesh);
		rtcReleaseGeometry(mesh);
	} // end of surfaces loop

    bvh_ = std::make_unique<BVH>(bvh_triangles);
    bvh_->BuildTree();

	rtcCommitScene(scene_);
}

int Raytracer::Ui()
{
    static float f = 0.0f;

    // Use a Begin/End pair to created a named window
    ImGui::Begin("Ray Tracer Params");

    ImGui::Text("Surfaces = %d", surfaces_.size());
    ImGui::Text("Materials = %d", materials_.size());
    ImGui::Separator();
    ImGui::Checkbox("Vsync", &vsync_);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
    //ImGui::ColorEdit3( "clear color", ( float* )&clear_color ); // Edit 3 floats representing a color

    // Buttons return true when clicked (most widgets return true when edited/activated)
    if (ImGui::Button("Left"))
        camera_.Rotate(-0.1f);

    ImGui::SameLine();
    if (ImGui::Button("Right"))
        camera_.Rotate(0.1f);

    ImGui::SameLine();
    if (ImGui::Button("Front"))
        camera_.Move(-20);

    ImGui::SameLine();
    if (ImGui::Button("Back"))
        camera_.Move(20);

    ImGui::NewLine();
    ImGui::Text("camera: %.1f %.1f", camera_.GetViewFrom().x, camera_.GetViewFrom().y);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    return 0;
}

bool Raytracer::is_visible(const Vector3 hit_point, const Vector3 light_point){

    // vector to light
    Vector3 l = light_point - hit_point;
    float dist = l.L2Norm();

    l *= 1.0f / dist;

    // to avoid self-shadowing
    Ray ray(hit_point, l, 0.001f);
    ray.set_tfar(dist);
    ray.intersect(scene_);
    return !ray.has_hit();
}

void Raytracer::LoadBackground() {
    background_ = std::make_unique<SphereMap>("data/royal_esplanade_4k.hdr");
}

Ray Raytracer::make_secondary_ray(const Vector3& origin, const Vector3& dir, const float ior) {
    return Ray{origin, dir, 0.001f, ior};
}

Color4f Raytracer::get_pixel(const int x, const int y, const float t)
{
//    int sample_count = 2;
//    Vector3 acc = {0, 0, 0};
//    for (int i = 0; i < sample_count; i++) {
//        for (int j = 0; j < sample_count; j++) {
//            float x_in = x + i * (1.0 / sample_count) + Random() / sample_count;
//            float y_in = y + j * (1.0 / sample_count) + Random() / sample_count;
//
//            auto rays = this->camera_.GenerateRays(x_in, y_in, 189, 1, 1.5);
//            Vector3 acc_d = {0, 0, 0};
//            for(auto& ray : rays){
//                Vector3 result = trace(ray, 0);
//                acc_d += result;
//            }
//            acc_d /= rays.size();
//            acc += acc_d;
//        }
//    }
//    acc /= (sample_count * sample_count);
//    return static_cast<Color4f>(acc);

    Ray ray(this->camera_.GenerateRay((float)x, (float)y));
    Vector3 result = trace(ray, 0);
    return static_cast<Color4f>(result);
}


Vector3 Raytracer::trace(Ray ray, const int depth = 0) {
    if(depth >= 10){
        return {0, 0, 0};
    }

    ray.intersect(scene_);

    if(Ray::BVH_BOOL){
        bvh_->Traverse(ray);
    }

    if (ray.has_hit())
    {

        Material* material = ray.get_material();
        Normal3f normal = ray.get_normal();

        const Vector3 hit_point = ray.get_hit_point();
        const Vector3 d = ray.get_direction();
        if(normal.DotProduct(d) > 0.0f){
            normal = normal*(-1.0f);
        }

        Vector3 l = omni_light_position_ - hit_point;
        normal.Normalize();
        l.Normalize();

        Vector3 v = -d;
        v.Normalize();

        Vector3 output_color;

        auto shaderId = static_cast<ShaderID>(material->shader_id);

        if(shaderId == ShaderID::Phong){
            return get_color_phong(ray, hit_point, omni_light_position_, normal, v, l, depth, material);
        }

        if(shaderId == ShaderID::Glass){
            return get_color_glass(ray, normal, v, l, depth, material);
        }

        if(is_visible(hit_point, omni_light_position_)){
            if(shaderId == ShaderID::Lambert){
                return get_color_lambert(ray, normal, l, material);
            }

            if(shaderId == ShaderID::Mirror){
                return get_color_mirror(ray, normal, v, l, depth, material);
            }

            //whitted
            float S = fabsf(normal.DotProduct(l));

            Vector3 v_r = v.Reflect(normal);
            v_r.Normalize();
            Ray secondary_ray = make_secondary_ray(hit_point, v_r, ray.get_ior());
            Vector3 L_i = trace(secondary_ray, depth + 1);

            Vector3 diffuse_color_v = ray.get_diffuse_color();
            Vector3 specular_color_v = ray.get_specular_color();
            output_color = (material->ambient + S * diffuse_color_v + L_i * specular_color_v);
        }
        else{
            output_color = material->ambient;
        }

        return output_color;
    }

    // ray did not hit any surface
    Vector3 ray_dir = ray.get_direction();
    Color3f bg_color = background_->texel(ray_dir.x, ray_dir.y, ray_dir.z);
    return {bg_color};
}

Vector3 Raytracer::get_color_lambert(Ray& ray, Vector3 normal, Vector3 l, Material* material){
    Vector3 result = material->ambient;
    Vector3 diffuse_color_v = ray.get_diffuse_color();

    Vector3 l_r = l.Reflect(normal);
    l_r.Normalize();
    float diff = fabsf(normal.DotProduct(l));
    result += (diff * diffuse_color_v);

    return result;
}

Vector3 Raytracer::get_color_phong(Ray& ray, Vector3 hit_point, Vector3 omni_light_position,
                                   Vector3 normal, Vector3 v, Vector3 l,
                                   int depth, Material* material) {

    Vector3 diffuse_color_v = ray.get_diffuse_color();
    Vector3 specular_color_v = ray.get_specular_color();

    v.Normalize();
    Vector3 v_r = v.Reflect(normal);
    v_r.Normalize();

    Vector3 c_phong = material->ambient;
    
    if(is_visible(hit_point, omni_light_position)){
        Vector3 l_r = l.Reflect(normal);
        l_r.Normalize();
        float diff = fabsf(normal.DotProduct(l));
        float phong_spec = powf(clamp(l_r.DotProduct(v)), material->shininess);
        c_phong += (diff * diffuse_color_v + phong_spec * specular_color_v);
    }

    float cos_fi = clamp(v_r.DotProduct(normal));
    float n1 = ray.get_ior();
    float n2 = material->ior;
    if(n1 == n2){
        n2 = IOR_AIR;
    }

    float F0 = (n1 - n2) / (n1 + n2);
    F0 = F0 * F0;
    float tmp = 1 - cos_fi;
    float R = F0 + (1 - F0) * (tmp * tmp * tmp * tmp * tmp);

    Ray secondary_ray = make_secondary_ray(hit_point, v_r, n1);
    Vector3 c_ref = trace(secondary_ray, depth + 1);
    return c_phong + c_ref * R;
}

Vector3 Raytracer::get_color_glass(Ray &ray, Vector3 normal, Vector3 v, Vector3 l, int depth, Material* material) {

    Vector3 hit_point = ray.get_hit_point();
    v.Normalize();
    normal.Normalize();

    float cos_fi = clamp(v.DotProduct(normal));
    float n1 = ray.get_ior();
    float n2 = material->ior;
    if(n1 == n2){
        n2 = IOR_AIR;
    }

    float F0 = (n1 - n2) / (n1 + n2);
    F0 = F0 * F0;
    float tmp = 1 - cos_fi;
    float R = F0 + (1 - F0) * (tmp * tmp * tmp * tmp * tmp);

    Vector3 v_r = v.Reflect(normal);
    Ray secondary_refl = make_secondary_ray(hit_point, v_r, n1);
    Vector3 c_refl = trace(secondary_refl, depth + 1);

    Vector3 v_t;
    Vector3 c_refr = {0, 0, 0};
    if(v.Refract(normal, n1, n2, v_t)){
        v_t.Normalize();
        Ray secondary_refr = make_secondary_ray(hit_point, v_t, n2);
        c_refr = trace(secondary_refr, depth + 1);
    }
    else{
        R = 1;
    }
    float T = 1 - R;

    Vector3 a = {1, 1, 1};
    if(n1 != IOR_AIR){
        a = T_b_l(material->attenuation, ray.get_tfar());
    }
    return (c_refl * R + c_refr * T) * a;
}

Vector3 Raytracer::get_color_mirror(Ray &ray, Vector3 normal, Vector3 v, Vector3 l,
                                    int depth, Material* material){

    Vector3 hit_point = ray.get_hit_point();

    float n1 = ray.get_ior();
    v.Normalize();
    normal.Normalize();

    Vector3 v_r = v.Reflect(normal);
    Ray secondary_refl = make_secondary_ray(hit_point, v_r, n1);
    Vector3 c_refl = trace(secondary_refl, depth + 1);

    return c_refl;
}