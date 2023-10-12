#include "stdafx.h"
#include "raytracer.h"
#include "objloader.h"
#include "tutorials.h"
#include "utils.h"

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
		} // end of triangles loop

		rtcCommitGeometry(mesh);
		unsigned int geom_id = rtcAttachGeometry(scene_, mesh);
		rtcReleaseGeometry(mesh);
	} // end of surfaces loop

	rtcCommitScene(scene_);
}

// bodov� svetlo: n = u*n1 + v*n2 + (1-u-v)*n3
// color=diff_color*cross(n*l)
float clamp(float x, float x0 = 0.0f, float x1 = 1.0f){
    return max(min(x, x0), x1);
}

bool Raytracer::is_visible(const Vector3 x, const Vector3 y){

    Vector3 l = y - x;
    float dist = l.L2Norm();

    l *= 1.0f / dist;

    RTCRay ray(x, l, 0.001f);
    RTCRayHit ray_hit(ray);

    // intersect ray with the scene
    RTCIntersectContext context{};
    rtcInitIntersectContext(&context);
    rtcIntersect1(scene_, &context, &ray_hit);

    return !ray_hit.has_hit();
}

RTCRay Raytracer::make_secondary_ray(const Vector3& origin, const Vector3& dir) {
    return {origin, dir, 0.001f};
}

Vector3 Raytracer::trace(RTCRay ray, const int depth = 0, const int max_depth = 10){
    if(depth >= max_depth) return Vector3();

    RTCRayHit ray_hit(ray);

    // intersect ray with the scene
    RTCIntersectContext context{};
    rtcInitIntersectContext(&context);
    rtcIntersect1(scene_, &context, &ray_hit);

    if (ray_hit.has_hit())
    {
        RTCGeometry geometry = rtcGetGeometry(scene_, ray_hit.hit.geomID);
        auto* material = (Material*) rtcGetGeometryUserData(geometry);
        assert(material);

        Normal3f normal;
        rtcInterpolate0(geometry, ray_hit.hit.primID, ray_hit.hit.u, ray_hit.hit.v,
                        RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &normal.x, 3);

        Coord2f tex_coord{};
        rtcInterpolate0(geometry, ray_hit.hit.primID, ray_hit.hit.u, ray_hit.hit.v,
                        RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, &tex_coord.u, 2);


        Color3f diffuse_color = Color3f(material->diffuse);
        Color3f specular_color = Color3f(material->specular);
        Texture * diffuse_texture = material->get_texture(Material::kDiffuseMapSlot);
        Texture * specular_texture = material->get_texture(Material::kSpecularMapSlot);
        if(diffuse_texture){
            Color3f diffuse_texel = diffuse_texture->get_texel(tex_coord.u, 1 - tex_coord.v);
            diffuse_color = diffuse_texel;
        }

        if(specular_texture){
            Color3f specular_texel = specular_texture->get_texel(tex_coord.u, 1 - tex_coord.v);
            specular_color = specular_texel;
        }

        Vector3 diffuse_color_v = Vector3(diffuse_color);
        Vector3 specular_color_v = Vector3(specular_color);
        const Vector3 omni_light_position{200, -500, 100};
        const Vector3 hit_point = ray_hit.get_hit_point();

        Vector3 d{ray.org_x, ray.org_y, ray.org_z};
        if(normal.DotProduct(d) > 0){
            normal = normal*(-1.0f);
        }

        Vector3 l = omni_light_position - hit_point;
        normal.Normalize();
        l.Normalize();

        Vector3 output;
        //Phong
        output += material->ambient;

        if(is_visible(hit_point, omni_light_position)){
            // TODO: fix
            output += diffuse_color_v * clamp((normal.DotProduct(l)));
            Vector3 l_r = 2 * (l.DotProduct(normal)) * normal - l;
            l_r.Normalize();

            RTCRay secondary_ray = make_secondary_ray(hit_point, l_r);

            Vector3 L_i = trace(secondary_ray, depth + 1);
            output += L_i * specular_color_v * powf(clamp(l_r.DotProduct(-d)), material->shininess);

        }
        return output;
    }

    //r g b a
    return {};
}

Color4f Raytracer::get_pixel(const int x, const int y, const float t)
{
	RTCRay ray = this->camera_.GenerateRay((float)x, (float)y);
    Vector3 result = trace(ray, 0, 5);
    return static_cast<Color4f>(result);
}

int Raytracer::Ui()
{
	static float f = 0.0f;
	static int counter = 0;

	// Use a Begin/End pair to created a named window
	ImGui::Begin("Ray Tracer Params");

	ImGui::Text("Surfaces = %d", surfaces_.size());
	ImGui::Text("Materials = %d", materials_.size());
	ImGui::Separator();
	ImGui::Checkbox("Vsync", &vsync_);

	//ImGui::Checkbox( "Demo Window", &show_demo_window ); // Edit bools storing our window open/close state
	//ImGui::Checkbox( "Another Window", &show_another_window );

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f    
	//ImGui::ColorEdit3( "clear color", ( float* )&clear_color ); // Edit 3 floats representing a color

	// Buttons return true when clicked (most widgets return true when edited/activated)
	if (ImGui::Button("Button"))
		counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	// 3. Show another simple window.
	/*if ( show_another_window )
	{
	ImGui::Begin( "Another Window", &show_another_window ); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	ImGui::Text( "Hello from another window!" );
	if ( ImGui::Button( "Close Me" ) )
	show_another_window = false;
	ImGui::End();
	}*/

	return 0;
}
