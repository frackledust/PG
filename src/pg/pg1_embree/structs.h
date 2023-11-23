#pragma once

struct Coord2f { float u, v; }; // texture coord structure

struct Triangle3ui { unsigned int v0, v1, v2; }; // indicies of a single triangle, the struct must match certain format, e.g. RTC_FORMAT_UINT3

struct Color3f {
    float r = 0, g = 0, b = 0;
    Color3f() = default;
    Color3f(float r, float g, float b): r(r), g(g), b(b) {}

    Color3f operator*(const float &f) const {
        return {r * f, g * f, b * f};
    }

    Color3f operator+(const Color3f &c) const {
        return {r + c.r, g + c.g, b + c.b};
    }
};

struct RTC_ALIGN( 16 ) Color4f
{


    struct { float r, g, b, a; }; // a = 1 means that the pixel is opaque

    Color4f(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    explicit Color4f(Color3f color, float a=1) : r(color.r), g(color.g), b(color.b), a(1.0f) {}

    static float compress(const float u,  float gamma);

    Color4f compress(float gamma = 2.4f){
        r = compress(r, gamma);
        g = compress(g, gamma);
        b = compress(b, gamma);
        return *this;
    }

    static float expand(const float u, float gamma);

    Color4f expand(float gamma = 2.4f){
        r = expand(r, gamma);
        g = expand(g, gamma);
        b = expand(b, gamma);
        return *this;
    }

    friend Color4f operator+( const Color4f & u, const Color4f & v ){
        return {u.r + v.r, u.g + v.g, u.b + v.b, u.a + v.a};
    }

    friend Color4f operator/( const Color4f & u, const float & v ){
        float a = 1.0f / v;
        return {u.r * a, u.g * a, u.b * a, u.a * a};
    }

    friend Color4f operator*( const Color4f & u, const float & a ){
        return {u.r * a, u.g * a, u.b * a, u.a * a};
    }
};
