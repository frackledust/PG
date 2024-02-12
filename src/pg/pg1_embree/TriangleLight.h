//
// Created by pavli on 01.12.2023.
//

#ifndef PG1_TRIANGLELIGHT_H
#define PG1_TRIANGLELIGHT_H


#include "BVH.h"
#include "utils.h"

class TriangleLight {
    float cdf;
public:
    std::shared_ptr<BVHTriangle> triangle;

    static bool NNE;

    Vector3 get_random_point(float &area, Vector3 &normal) const {
        float r1 = Random(0, 1);
        float r2 = Random(0, 1);

        float sqrt_r1 = sqrt(r1);

        float u = 1 - sqrt_r1;
        float v = sqrt_r1 * (1 - r2);
        float w = sqrt_r1 * r2;

        Vector3 p0 = triangle->vertices_[0].position;
        Vector3 p1 = triangle->vertices_[1].position;
        Vector3 p2 = triangle->vertices_[2].position;

        area = triangle->area;
        normal = triangle->get_normal(u, v);

        return p0 * u + p1 * v + p2 * w;
    }

    Vector3 get_color() const {
        if(triangle->material->emission < Vector3(0, 0, 0)){
            return {1, 1, 1};
        }
        return triangle->material->emission;
    }

    static void calculate_cdf(std::vector<std::shared_ptr<TriangleLight>> lights);

    static std::shared_ptr<TriangleLight> get_light(float value, std::vector<std::shared_ptr<TriangleLight>> lights);

    static float TotalLightArea;
};


#endif //PG1_TRIANGLELIGHT_H
