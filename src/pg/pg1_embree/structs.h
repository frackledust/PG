#pragma once

#include "vector3.h"

struct Vector3;

struct Coord2f { float u, v; }; // texture coord structure

struct Triangle3ui { unsigned int v0, v1, v2; }; // indicies of a single triangle, the struct must match certain format, e.g. RTC_FORMAT_UINT3

struct Color3f {
    float r, g, b;

    Color3f(float r, float g, float b): r(r), g(g), b(b) {}
};

struct RTC_ALIGN( 16 ) Color4f
{


    struct { float r, g, b, a; }; // a = 1 means that the pixel is opaque

    Color4f(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    explicit Color4f(Color3f color, float a=1) : r(color.r), g(color.g), b(color.b), a(1.0f) {}
};
