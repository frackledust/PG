
// Created by pavli on 19.10.2023.
#include "stdafx.h"
#include "SphereMap.h"

SphereMap::SphereMap(const std::string &file_name) {
    texture_ = std::make_unique<Texture>(file_name.c_str());
}

Color3f SphereMap::texel(const float x, const float y, const float z) {
     const float u = atan2f(y, x) / float(M_PI) + ((y < 0.0f) ? float(M_PI) * 2.0f : 0.0f);
     const float v = acosf(z) / float(M_PI);
     return texture_->get_texel(u, v);
}