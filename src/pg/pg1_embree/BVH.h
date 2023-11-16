//
// Created by pavli on 11.11.2023.
//

#ifndef PG1_BVH_H
#define PG1_BVH_H

#include "triangle.h"
#include "ray.h"
#include <vector>
#include <memory>

class BVHBbox{
public:
    Vector3 border_min;
    Vector3 border_max;
    BVHBbox(){
        border_min = Vector3(0, 0,  0);
        border_max = Vector3(0, 0,  0);
    }

    bool is_intersecting(Ray &ray) const {
        // SLAB method
        float tx0 = (border_min.x - ray.get_origin().x) / ray.get_direction().x;
        float tx1 = (border_max.x - ray.get_origin().x) / ray.get_direction().x;
        float ty0 = (border_min.y - ray.get_origin().y) / ray.get_direction().y;
        float ty1 = (border_max.y - ray.get_origin().y) / ray.get_direction().y;
        float tz0 = (border_min.z - ray.get_origin().z) / ray.get_direction().z;
        float tz1 = (border_max.z - ray.get_origin().z) / ray.get_direction().z;

        if (tx0 > tx1) std::swap(tx0, tx1);
        if (ty0 > ty1) std::swap(ty0, ty1);
        if (tz0 > tz1) std::swap(tz0, tz1);

        float tmin = max(max(tx0, ty0), tz0);
        float tmax = min(min(tx1, ty1), tz1);

        return tmin <= tmax && tmax >= 0;
    }
};

class BVHTriangle {
public:
    Vertex vertices_[3];
    unsigned int geom_id;
    std::shared_ptr<BVHBbox> bbox = nullptr;
    Vector3 edge1;
    Vector3 edge2;

    BVHTriangle(Vertex a, Vertex b, Vertex c){
        vertices_[0] = a;
        vertices_[1] = b;
        vertices_[2] = c;
        edge1 = vertices_[1].position - vertices_[0].position;
        edge2 = vertices_[2].position - vertices_[0].position;
    }

    Vector3 get_center(){
        return (vertices_[0].position + vertices_[1].position + vertices_[2].position) / 3.0f;
    }

    void calculate_bbox() {
        bbox = std::make_shared<BVHBbox>();
        Vector3 v_min = vertices_[0].position;
        Vector3 v_max = vertices_[0].position;

        for(int j = 1; j < 3; j++){
            Vector3 vertex_pos = vertices_[j].position;
            for(int k = 0; k < 3; k++){
                v_min[k] = min(v_min[k], vertex_pos[k]);
                v_max[k] = max(v_max[k], vertex_pos[k]);
            }
        }

        bbox->border_min = v_min;
        bbox->border_max = v_max;
    }

    std::shared_ptr<BVHBbox> get_bbox(){
        if(bbox == nullptr){
            calculate_bbox();
        }
        return bbox;
    }

    void is_intersected(Ray &ray) {
        // Moller-Trumbore algorithm
        Vector3 ray_dir = ray.get_direction();
        Vector3 h = ray_dir.CrossProduct(edge2);

        float a = edge1.DotProduct(h);
        if (a > -0.00001 && a < 0.00001)
            return;

        float f = 1.0 / a;
        Vector3 s = ray.get_origin() - vertices_[0].position;

        float u = f * s.DotProduct(h);
        if (u < 0.0 || u > 1.0)
            return;
        Vector3 q = s.CrossProduct(edge1);

        float v = f * ray_dir.DotProduct(q);
        if (v < 0.0 || u + v > 1.0)
            return;

        float t = f * edge2.DotProduct(q);
        if (t > ray.get_tnear())
        {
            if (t < ray.bvh_tfar) {
                ray.bvh_tfar = t;
                ray.bvh_intersected = true;
                ray.bvh_geom_id = this->geom_id;
            }
        }
    }

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
