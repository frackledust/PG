//
// Created by pavli on 11.11.2023.
//

#ifndef PG1_BVH_H
#define PG1_BVH_H

#include "ray.h"
#include "triangle.h"
#include "material.h"
#include <vector>
#include <memory>


class Ray;
class Material;

class BVHBbox{
public:
    Vector3 border_min;
    Vector3 border_max;
    BVHBbox(){
        border_min = Vector3(0, 0,  0);
        border_max = Vector3(0, 0,  0);
    }

    bool is_intersecting(Ray &ray);
};

class BVHHitPoint{
public:
    bool is_intersected = false;
    float tfar = FLT_MAX;
    float u = FLT_MAX;
    float v = FLT_MAX;
    Vector3 normal = {FLT_MAX, FLT_MAX, FLT_MAX};
    Coord2f text_coords;
    Material* material;
    BVHHitPoint() = default;
};

class BVHTriangle {
public:
    Vertex vertices_[3];
    unsigned int geom_id;
    Material* material;
    std::shared_ptr<BVHBbox> bbox = nullptr;
    float area = 0;

    BVHTriangle(Vertex a, Vertex b, Vertex c, unsigned int geom_id, Material* material){
        this->vertices_[0] = a;
        this->vertices_[1] = b;
        this->vertices_[2] = c;
        this->geom_id = geom_id;
        this->material = material;

        this->calculate_bbox();
        this->calculate_area();
    }

    Vector3 get_center(){
        return (vertices_[0].position + vertices_[1].position + vertices_[2].position) / 3.0f;
    }

    Vector3 get_normal(float u, float v){
        return vertices_[0].normal * (1 - u - v) + vertices_[1].normal * u + vertices_[2].normal * v;
    }

    Coord2f get_coords(float u, float v){
        Coord2f coords;
        coords.u = vertices_[0].texture_coords[0].u * (1 - u - v) + vertices_[1].texture_coords[0].u * u
                + vertices_[2].texture_coords[0].u * v;
        coords.v = vertices_[0].texture_coords[0].v * (1 - u - v) + vertices_[1].texture_coords[0].v * u
                   + vertices_[2].texture_coords[0].v * v;
        return coords;
    }

    void calculate_area();
    void calculate_bbox();

    std::shared_ptr<BVHBbox> get_bbox(){
        if(bbox == nullptr){
            calculate_bbox();
        }
        return bbox;
    }

    void is_intersected(Ray &ray);

};

class BVHNode {
public:
    std::shared_ptr<BVHBbox> bbox;
    int span [2]{};
    std::shared_ptr<BVHNode> children[2];

    BVHNode(int from, int to){
        bbox = nullptr;
        span[0] = from;
        span[1] = to;
        children[0] = nullptr;
        children[1] = nullptr;
    }

    bool is_leaf(){
        return children[0] == nullptr && children[1] == nullptr;
    }
};

class BVH {
public:
    explicit BVH(std::vector<std::shared_ptr<BVHTriangle>> items);


    void BuildTree();
    void Traverse(Ray &ray);

private:
    std::shared_ptr<BVHNode> root_;
    std::vector<std::shared_ptr<BVHTriangle>> items_;
    int max_leaf_items_;

    std::shared_ptr<BVHNode> BuildTree(int from, int to, int depth);
    void Traverse(Ray &ray, const std::shared_ptr<BVHNode>& node, int depth);
    std::shared_ptr<BVHBbox> CalculateNodeBounds(int from, int to);

};


#endif //PG1_BVH_H
