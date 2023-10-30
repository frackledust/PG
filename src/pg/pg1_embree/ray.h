//
// Created by pavli on 15.10.2023.
//

#ifndef PG1_RAY_H
#define PG1_RAY_H


#include <cfloat>
#include <memory>
#include "embree3/rtcore_ray.h"

class Ray {

public:
std::shared_ptr<RTCScene> scene;
    Ray(){
        ray_hit.ray.org_x = 0.0f;
        ray_hit.ray.org_y = 0.0f;
        ray_hit.ray.org_z = 0.0f;
        ray_hit.ray.dir_x = 0.0f;
        ray_hit.ray.dir_y = 0.0f;
        ray_hit.ray.dir_z = 0.0f;
        ray_hit.ray.tnear = 0.0f;
        ray_hit.ray.tfar = FLT_MAX;
        ray_hit.ray.time = 0.0f;
        ray_hit.ray.mask = 0;
        ray_hit.ray.id = 0;
        ray_hit.ray.flags = 0;
    }

    Ray(RTCRay _ray){
        ray_hit.ray = _ray;
    }

    Ray(Vector3 org, Vector3 dir, float _tnear, float time_ = 0.0f){
        ray_hit.ray.org_x = org.x;
        ray_hit.ray.org_y = org.y;
        ray_hit.ray.org_z = org.z;
        ray_hit.ray.dir_x = dir.x;
        ray_hit.ray.dir_y = dir.y;
        ray_hit.ray.dir_z = dir.z;
        ray_hit.ray.tnear = _tnear;
        ray_hit.ray.tfar = FLT_MAX;
        ray_hit.ray.time = time_;
        ray_hit.ray.mask = 0;
        ray_hit.ray.id = 0;
        ray_hit.ray.flags = 0;
    }

    RTCRayHit ray_hit;

    void init_ray_hit(){
        ray_hit.hit.Ng_x = 0.0f;
        ray_hit.hit.Ng_y = 0.0f;
        ray_hit.hit.Ng_z = 0.0f;
        ray_hit.hit.u = 0.0f;
        ray_hit.hit.v = 0.0f;
        ray_hit.hit.primID = RTC_INVALID_GEOMETRY_ID;
        ray_hit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    }

    void intersect(RTCScene _scene);

    RTCGeometry get_geometry();

    Normal3f get_normal();

    bool has_hit() const {
        return ray_hit.hit.geomID != RTC_INVALID_GEOMETRY_ID;
    }

    Vector3 get_hit_point() const{
        RTCRay ray = ray_hit.ray;
        return {ray.org_x + ray.dir_x * ray.tfar,
                ray.org_y + ray.dir_y * ray.tfar,
                ray.org_z + ray.dir_z * ray.tfar};
    }

    Coord2f get_texture_coord();

    Vector3 get_direction() const;

    float get_ior() const{
        return ray_hit.ray.time;
    }

    void set_tfar(float tfar_);

    float get_tfar();
};


#endif //PG1_RAY_H
