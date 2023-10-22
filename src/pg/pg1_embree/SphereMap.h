//
// Created by pavli on 19.10.2023.
//

#ifndef PG1_SPHEREMAP_H
#define PG1_SPHEREMAP_H
#include <string>
#include <memory>
#define _USE_MATH_DEFINES
#include <math.h>
#include "texture.h"

class SphereMap {
public:
    std::unique_ptr<Texture> texture_;

    SphereMap(const std::string& file_name);
    Color3f texel(float x, float y, float z);
};


#endif //PG1_SPHEREMAP_H
