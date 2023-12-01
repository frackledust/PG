//
// Created by pavli on 01.12.2023.
//

#include <iostream>
#include "TriangleLight.h"

bool TriangleLight::NNE = false;

float TriangleLight::TotalLightArea = 0;

void TriangleLight::calculate_cdf(std::vector<std::shared_ptr<TriangleLight>> lights) {
    float sum = 0;
    for(auto &light : lights){
        sum += light->triangle->area;
    }

    TotalLightArea = sum;

    for(auto &light : lights){
        light->cdf = light->triangle->area / sum;
    }

    for(int i = 1; i < lights.size(); i++){
        lights[i]->cdf += lights[i - 1]->cdf;
    }
}

std::shared_ptr<TriangleLight> TriangleLight::get_light(float value, std::vector<std::shared_ptr<TriangleLight>> lights) {
    for(auto &light : lights){
        if(light->cdf > value){
            return light;
        }
    }
    return lights[lights.size() - 1];
}
