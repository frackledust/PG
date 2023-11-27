#include "stdafx.h"
#include "pathtracer.h"
#include "objloader.h"
#include "tutorials.h"
#include "ray.h"
#include "SphereMap.h"
#include "BVH.h"
#include "mymath.h"
#include "utils.h"

Pathtracer::Pathtracer(const int width, const int height,
	const float fov_y, const Vector3 view_from, const Vector3 view_at,
	const char* config) : SimpleGuiDX11(width, height)
{
	InitDeviceAndScene(config);
    buffer_data = new float[width*height * 3];
    buffer_count = new int[width*height];
    memset(buffer_data, 0, width*height * 3 * sizeof(float));
    memset(buffer_count, 0, width*height * sizeof(int));
	camera_ = Camera(width, height, fov_y, view_from, view_at);
}

Pathtracer::~Pathtracer()
{
	ReleaseDeviceAndScene();
}

int Pathtracer::InitDeviceAndScene(const char* config)
{
	device_ = rtcNewDevice(config);
	error_handler(nullptr, rtcGetDeviceError(device_), "Unable to create a new device.\n");
	rtcSetDeviceErrorFunction(device_, error_handler, nullptr);

	ssize_t triangle_supported = rtcGetDeviceProperty(device_, RTC_DEVICE_PROPERTY_TRIANGLE_GEOMETRY_SUPPORTED);

	// create a new scene bound to the specified device
	scene_ = rtcNewScene(device_);

	return S_OK;
}

int Pathtracer::ReleaseDeviceAndScene()
{
	rtcReleaseScene(scene_);
	rtcReleaseDevice(device_);

	return S_OK;
}

void Pathtracer::LoadScene(const std::string file_name)
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

int Pathtracer::Ui()
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

bool Pathtracer::is_visible(const Vector3 hit_point, const Vector3 light_point){

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

void Pathtracer::LoadBackground() {
    background_ = std::make_unique<SphereMap>("data/royal_esplanade_4k.hdr");
}

Ray Pathtracer::make_secondary_ray(const Vector3& origin, const Vector3& dir, const float ior) {
    return Ray{origin, dir, 0.01f, ior};
}

Color4f Pathtracer::get_pixel(const int x, const int y, const float t)
{
    int sample_count = 50;
    int total_samples = sample_count * sample_count;

    Vector3 acc = {0, 0, 0};
    for (int i = 0; i < sample_count; i++) {
        for (int j = 0; j < sample_count; j++) {
            float x_in = x + i * (1.0 / sample_count) + Random() / sample_count;
            float y_in = y + j * (1.0 / sample_count) + Random() / sample_count;

            Ray ray(this->camera_.GenerateRay((float)x_in, (float)y_in));
            Vector3 result = trace(ray, 0);
            acc += result;
        }
    }

    float * data = buffer_data + (y * width_ + x) * 3;
    Vector3 buffered_color = {data[0], data[1], data[2]};
    buffered_color += acc;

    int * count = buffer_count + (y * width_ + x);
    int buffered_count = *count + total_samples;

    data[0] = buffered_color.x;
    data[1] = buffered_color.y;
    data[2] = buffered_color.z;
    count[0] = buffered_count;

    Vector3 final_color = buffered_color / buffered_count;
    return static_cast<Color4f>(final_color);
}


Vector3 Pathtracer::trace(Ray ray, const int depth = 0) {
    if(depth >= 5){
        return {1, 0, 0};
    }

    ray.intersect(scene_);
    if(Ray::BVH_BOOL){
        bvh_->Traverse(ray);
    }

    if(!ray.has_hit()){
        Vector3 ray_dir = ray.get_direction();
        //Color3f bg_color = background_->texel(ray_dir.x, ray_dir.y, ray_dir.z);
        //return {bg_color};
        return {1, 1, 1};
    }

    Vector3 d = ray.get_direction();
    d.Normalize();
    Vector3 v = -d;

    Normal3f normal = ray.get_normal();
    normal.Normalize();
    if(normal.DotProduct(d) > 0.0f){
        normal = normal*(-1.0f);
    }

    Material *material = ray.get_material();
    Vector3 emission = material->emission;
    if (emission.x > 0 || emission.y > 0 || emission.z > 0) {
        return emission;
    }

    if(material->shader_id == ShaderID::Mirror){
        Vector3 omega_i = v.Reflect(normal);
        omega_i.Normalize();
        Ray secondary_ray = make_secondary_ray(ray.get_hit_point(), omega_i, IOR_AIR);
        Vector3 L_i = trace(secondary_ray, depth + 1);
        return material->reflectivity * L_i;
    }
/*
    if(material->shader_id == ShaderID::Phong){
        float pdf;
        Vector3 omega_i = sample_hemisphere(normal, pdf);
        Ray secondary_ray = make_secondary_ray(ray.get_hit_point(), omega_i, IOR_AIR);
        Vector3 L_i = trace(secondary_ray, depth + 1);

        float cos_theta = omega_i.DotProduct(normal);

        Vector3 omega_r = v.Reflect(normal);
        float cos_theta_r = clamp(omega_i.DotProduct(omega_r));

        Vector3 f_r = material->diffuse / M_PI
                      + material->specular * (material->shininess + 2) / (2 * M_PI)
                        * powf(cos_theta_r, material->shininess);
        Vector3 L_r = L_i * f_r * (cos_theta) / ( pdf );
        return L_r;
    }
*/

    if(material->shader_id == ShaderID::Phong){
        Vector3 diffuse_color = {1, 1, 1};
        Vector3 specular_color = {1, 1, 1};



        // BRDF-proportional importance sampling
        float diffuse_max = diffuse_color.LargestComponentValue();
        float specular_max = specular_color.LargestComponentValue();
        float random_v = Random(0, diffuse_max + specular_max);

        if(random_v < diffuse_max){
            float pdf = 1;
            Vector3 omega_i = sample_cosine_hemisphere(normal, pdf);
            omega_i.Normalize();

            Ray secondary_ray = make_secondary_ray(ray.get_hit_point(), omega_i, IOR_AIR);
            Vector3 L_i = trace(secondary_ray, depth + 1);
            pdf*= diffuse_max / (diffuse_max + specular_max);

            Vector3 f_r = diffuse_color / M_PI;
            float cos_theta = omega_i.DotProduct(normal);

            Vector3 L_r = L_i * f_r * (cos_theta) / ( pdf );
            return L_r;
        }
        else{
            float pdf = 1;
            Vector3 r = v.Reflect(normal);
            Vector3 omega_i = sample_cosine_lobe(r, material->shininess, pdf);
            omega_i.Normalize();

            if (omega_i.DotProduct(normal) < 0.0f){
                return {0, 0, 0};
            }

            Ray secondary_ray = make_secondary_ray(ray.get_hit_point(), omega_i, IOR_AIR);
            Vector3 L_i = trace(secondary_ray, depth + 1);

            pdf*= specular_max / (diffuse_max + specular_max);

            float cos_theta = omega_i.DotProduct(normal);
//            version 2?
//            float cos_theta = omega_i.DotProduct(normal);
//            Vector3 omega_r = v.Reflect(normal);
//            float cos_theta_r = clamp(omega_i.DotProduct(omega_r));


            Vector3 f_r = specular_color * ((material->shininess) + 2 / 2 * M_PI);
//            Vector3 f_r = specular_color * ((material->shininess) + 2 / 2 * M_PI) * powf(cos_theta_r, material->shininess);

            Vector3 L_r = L_i * f_r * (cos_theta) / ( pdf );
            return L_r;
        }
    }

    // LAMBERT
    Vector3 diffuse_color = material->diffuse;
    // russian roulette
    float alpha = diffuse_color.LargestComponentValue();
//    alpha = 1;
    if(alpha <= Random(0, 1)){
        return {0, 0, 0};
    }

    float pdf;
    Vector3 omega_i = sample_cosine_hemisphere(normal, pdf);
    omega_i.Normalize();
    Ray secondary_ray = make_secondary_ray(ray.get_hit_point(), omega_i, IOR_AIR);
    Vector3 L_i = trace(secondary_ray, depth + 1);

    float cos_theta = omega_i.DotProduct(normal);
    Vector3 f_r = diffuse_color / M_PI;
    Vector3 L_r = L_i * f_r * (cos_theta) / ( pdf * alpha);
    return L_r;
}

Vector3 Pathtracer::sample_hemisphere(Normal3f normal, float &pdf) {
    float r1 = Random(0, 1);
    float r2 = Random(0, 1);

    float sin_theta = sqrt(1 - r2 * r2);
    float phi = 2 * M_PI * r1;

    float x = 2 * cos(phi) * sin_theta;
    float y = 2 * sin(phi) * sin_theta;
    float z = r2;

    Vector3 result = {x, y, z};
    result.Normalize();

    if (normal.DotProduct(result) < 0.0f){
        result = result * (-1.0f);
    }


    pdf = 1 / (2 * M_PI);
    return result;
}

Vector3 Pathtracer::sample_cosine_hemisphere(Normal3f normal, float &pdf) {
    float r1 = Random(0, 1);
    float r2 = Random(0, 1);

    float sin_theta = sqrtf(1 - r1 * r1);
    float phi = 2 * M_PI * r2;

    float x = sin_theta * cosf(phi);
    float y = sin_theta * sinf(phi);
    float z = r1;

    Vector3 result = {x, y, z};
    result.Normalize();

    // Transformation from Local Reference Frame to World Reference Frame
    Vector3 o2 = orthogonal(normal);
    Vector3 o1 = o2.CrossProduct(normal);
    Matrix3 T_rs = Matrix3x3(o1, o2, normal);

    // Switch rows and columns
    T_rs.Transpose();

    float cos_theta = result.DotProduct(normal);
    pdf = cos_theta / M_PI;

    return T_rs * result;
}

Vector3 Pathtracer::sample_cosine_lobe(Vector3 normal, float gamma, float &pdf) {
    float r1 = Random(0, 1);
    float r2 = Random(0, 1);

    float x = cos(2 * M_PI * r1) * sqrt(1 - pow(r2, 2 / gamma + 1));
    float y = sin(2 * M_PI * r1) * sqrt(1 - pow(r2, 2 / gamma + 1));
    float z = pow(r2, 1 / gamma + 1);

    Vector3 result = {x, y, z};
    result.Normalize();

//    Vector3 o2 = orthogonal(normal);
//    Vector3 o1 = o2.CrossProduct(normal);
//    Matrix3 T_rs = Matrix3x3(o1, o2, normal);
//
//    // Switch rows and columns
//    T_rs.Transpose();
//    result = T_rs * result;

    pdf = (gamma + 1) / (2 * M_PI) * pow(result.z, gamma);

    return result;
}
