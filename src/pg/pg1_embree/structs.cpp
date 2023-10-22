#include "stdafx.h"
#include "structs.h"


float Color4f::compress(const float u, float gamma) {
    if (u <= 0) return 0;
    if (u == 1) return 1.0f;
    if (u <= 0.0031308f) return 12.92f * u;
    return (1.055f * powf(u, 1 / gamma)) - 0.055;
}

float Color4f::expand(const float u, float gamma) {
    if (u <= 0) return 0.0f;
    if (u >= 1) return 1.0f;
    if (u <= 0.04045) return u / 12.92f;
    return powf((u + 0.055f) / 1.055f, gamma);
}


