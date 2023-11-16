//
// Created by pavli on 15.10.2023.
//

#include "ray.h"
#include "embree3/rtcore_scene.h"

bool Ray::BVH_BOOL = false;

void Ray::intersect(RTCScene _scene) {
    init_ray_hit();
    this->scene = std::make_shared<RTCScene>(_scene);
    RTCIntersectContext context{};
    rtcInitIntersectContext(&context);

    if(BVH_BOOL){
    }
    else{
        rtcIntersect1(_scene, &context, &ray_hit);
    }
}

RTCGeometry Ray::get_geometry() {
    if(BVH_BOOL){
        return rtcGetGeometry(*scene, bvh_geom_id);
    }
    return rtcGetGeometry(*scene, ray_hit.hit.geomID);
}

void *Ray::get_material() {
    if(BVH_BOOL){
        return bvh_material;
    }
    RTCGeometry geometry = get_geometry();
    return rtcGetGeometryUserData(geometry);
}

Normal3f Ray::get_normal() {
    if(BVH_BOOL){
        return bvh_normal;
    }

    Normal3f normal{};
    RTCGeometry geometry = get_geometry();
    rtcInterpolate0(geometry, ray_hit.hit.primID, ray_hit.hit.u, ray_hit.hit.v,
                    RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &normal.x, 3);
    return normal;
}

Coord2f Ray::get_texture_coord() {
    Coord2f tex_coord{};
    if(BVH_BOOL){
        return bvh_text_coords;
    }

    RTCGeometry geometry = get_geometry();
    rtcInterpolate0(geometry, ray_hit.hit.primID, ray_hit.hit.u, ray_hit.hit.v,
                    RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, &tex_coord.u, 2);
    return tex_coord;
}

Vector3 Ray::get_origin() const {
    return Vector3(ray_hit.ray.org_x, ray_hit.ray.org_y, ray_hit.ray.org_z);
}

Vector3 Ray::get_direction() const {
    return Vector3(ray_hit.ray.dir_x, ray_hit.ray.dir_y, ray_hit.ray.dir_z);
}

void Ray::set_tfar(float tfar_) {
    ray_hit.ray.tfar = tfar_;
}

float Ray::get_tfar() {
    if(BVH_BOOL){
        return bvh_tfar;
    }
    return ray_hit.ray.tfar;
}

float Ray::get_tnear() {
    return ray_hit.ray.tnear;
}

bool Ray::has_hit() const {
    if(BVH_BOOL){
        return bvh_intersected;
    }
    return ray_hit.hit.geomID != RTC_INVALID_GEOMETRY_ID;
}


