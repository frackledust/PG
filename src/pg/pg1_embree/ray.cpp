//
// Created by pavli on 15.10.2023.
//

#include "ray.h"
#include "embree3/rtcore_scene.h"

void Ray::intersect(RTCScene _scene) {
    init_ray_hit();
    this->scene = std::make_shared<RTCScene>(_scene);
    RTCIntersectContext context{};
    rtcInitIntersectContext(&context);
    rtcIntersect1(_scene, &context, &ray_hit);
}

RTCGeometry Ray::get_geometry() {
    return rtcGetGeometry(*scene, ray_hit.hit.geomID);
}

Normal3f Ray::get_normal() {
    Normal3f normal{};
    RTCGeometry geometry = get_geometry();
    rtcInterpolate0(geometry, ray_hit.hit.primID, ray_hit.hit.u, ray_hit.hit.v,
                    RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &normal.x, 3);
    return normal;
}

Coord2f Ray::get_texture_coord() {
    Coord2f tex_coord{};
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
    return ray_hit.ray.tfar;
}


