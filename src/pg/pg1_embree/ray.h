//
// Created by pavli on 15.10.2023.
//

#ifndef PG1_RAY_H
#define PG1_RAY_H


#include <cfloat>
#include <memory>
#include "embree3/rtcore_ray.h"
#include "material.h"
#include "BVH.h"

class BVHHitPoint;

class Ray {

public:
    static bool BVH_BOOL;

    RTCRayHit ray_hit;
    std::shared_ptr<RTCScene> scene;
    std::shared_ptr<BVHHitPoint> bvh_hit_point;

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

        ray_hit.hit.Ng_x = 0.0f;
        ray_hit.hit.Ng_y = 0.0f;
        ray_hit.hit.Ng_z = 0.0f;
        ray_hit.hit.u = 0.0f;
        ray_hit.hit.v = 0.0f;
        ray_hit.hit.primID = RTC_INVALID_GEOMETRY_ID;
        ray_hit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    }

    Ray(RTCRay _ray){
        ray_hit.ray = _ray;
    }

    Ray(Vector3 org, Vector3 dir, float tnear_, float time_ = 0.0f){
        ray_hit = RTCRayHit();
        ray_hit.ray.org_x = org.x;
        ray_hit.ray.org_y = org.y;
        ray_hit.ray.org_z = org.z;
        ray_hit.ray.dir_x = dir.x;
        ray_hit.ray.dir_y = dir.y;
        ray_hit.ray.dir_z = dir.z;
        ray_hit.ray.tnear = tnear_;
        ray_hit.ray.tfar = FLT_MAX;
        ray_hit.ray.time = time_;
        ray_hit.ray.mask = 0;
        ray_hit.ray.id = 0;
        ray_hit.ray.flags = 0;
        ray_hit.hit.Ng_x = 0.0f;
        ray_hit.hit.Ng_y = 0.0f;
        ray_hit.hit.Ng_z = 0.0f;
        ray_hit.hit.u = 0.0f;
        ray_hit.hit.v = 0.0f;
        ray_hit.hit.primID = RTC_INVALID_GEOMETRY_ID;
        ray_hit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        if(BVH_BOOL){
            bvh_hit_point = std::make_shared<BVHHitPoint>();
        }
    }

    void intersect(RTCScene _scene);

    Normal3f get_normal() const;

    bool has_hit() const;

    Vector3 get_hit_point() const;

    Coord2f get_texture_coord() const;

    Vector3 get_origin() const;

    Vector3 get_direction() const;

    float get_ior() const;

    void set_tfar(float tfar_);

    float get_tfar() const;

    float get_tnear() const;

    Material * get_material() const;

    Vector3 get_diffuse_color() const;

    Vector3 get_specular_color() const;
};


#endif //PG1_RAY_H
