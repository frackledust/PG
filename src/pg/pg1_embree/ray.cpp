//
// Created by pavli on 15.10.2023.
//

#include "ray.h"
#include "embree3/rtcore_scene.h"

bool Ray::BVH_BOOL = false;

void Ray::intersect(RTCScene _scene) {
    this->scene = std::make_shared<RTCScene>(_scene);
    RTCIntersectContext context{};
    rtcInitIntersectContext(&context);

    if(BVH_BOOL){
    }
    else{
        rtcIntersect1(_scene, &context, &ray_hit);
    }
}

Material * Ray::get_material() const {
    if(BVH_BOOL){
        return bvh_hit_point->material;
    }

    RTCGeometry geometry = rtcGetGeometry(*scene, ray_hit.hit.geomID);
    return (Material*) rtcGetGeometryUserData(geometry);
}

Normal3f Ray::get_normal() const {
    if(BVH_BOOL){
        return bvh_hit_point->normal;
    }

    Normal3f normal{};
    RTCGeometry geometry = rtcGetGeometry(*scene, ray_hit.hit.geomID);
    rtcInterpolate0(geometry, ray_hit.hit.primID, ray_hit.hit.u, ray_hit.hit.v,
                    RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &normal.x, 3);
    return normal;
}

Coord2f Ray::get_texture_coord() const {
    Coord2f tex_coord{};
    if(BVH_BOOL){
        return bvh_hit_point->text_coords;
    }

    RTCGeometry geometry = rtcGetGeometry(*scene, ray_hit.hit.geomID);
    rtcInterpolate0(geometry, ray_hit.hit.primID, ray_hit.hit.u, ray_hit.hit.v,
                    RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, &tex_coord.u, 2);
    return tex_coord;
}

Vector3 Ray::get_diffuse_color() const {
    Coord2f tex_coord = get_texture_coord();
    return {get_material()->get_diffuse_color(tex_coord.u, tex_coord.v)};
}

Vector3 Ray::get_specular_color() const {
    Coord2f tex_coord = get_texture_coord();
    return {get_material()->get_specular_color(tex_coord.u, tex_coord.v)};
}

Vector3 Ray::get_origin() const {
    return Vector3(ray_hit.ray.org_x, ray_hit.ray.org_y, ray_hit.ray.org_z);
}

Vector3 Ray::get_direction() const {
    return Vector3(ray_hit.ray.dir_x, ray_hit.ray.dir_y, ray_hit.ray.dir_z);
}

float Ray::get_ior() const {
    return ray_hit.ray.time;
};

void Ray::set_tfar(float tfar_) {
    ray_hit.ray.tfar = tfar_;
    if(BVH_BOOL){
        bvh_hit_point->tfar = tfar_;
    }
}

float Ray::get_tfar() const {
    if(BVH_BOOL){
        return bvh_hit_point->tfar;
    }
    return ray_hit.ray.tfar;
}

float Ray::get_tnear() const {
    return ray_hit.ray.tnear;
}

bool Ray::has_hit() const {
    if(BVH_BOOL){
        return bvh_hit_point->is_intersected;
    }
    return ray_hit.hit.geomID != RTC_INVALID_GEOMETRY_ID;
}

Vector3 Ray::get_hit_point() const {
    float _tfar = get_tfar();
    return {
            ray_hit.ray.org_x + _tfar * ray_hit.ray.dir_x,
            ray_hit.ray.org_y + _tfar * ray_hit.ray.dir_y,
            ray_hit.ray.org_z + _tfar * ray_hit.ray.dir_z
    };
}


