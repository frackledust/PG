//
// Created by pavli on 11.11.2023.
//
#include "stdafx.h"
#include "BVH.h"

#include <utility>

BVH::BVH(std::vector<std::shared_ptr<BVHTriangle>> items) {
    items_ = std::move(items);
    max_leaf_items_ = 4;
}

void BVH::BuildTree() {
    root_ = BuildTree(0, items_.size(), 0);
}

void BVH::Traverse(Ray &ray) {
    Traverse(ray, root_, 0);
}

std::shared_ptr <BVHNode> BVH::BuildTree(int from, int to, int depth) {
    auto node = std::make_shared<BVHNode>(from, to);
    node->bbox = CalculateNodeBounds(from, to);
    if (to - from + 1 > max_leaf_items_){
        int axis = depth % 3;
        int pivot = (from + to) / 2;

        std::nth_element(items_.begin() + from, items_.begin() + pivot, items_.begin() + to,
                         [axis](const std::shared_ptr<BVHTriangle> &a, const std::shared_ptr<BVHTriangle> &b) {
                             return a->get_center()[axis] < b->get_center()[axis];
                         });

        node->children[0] = BuildTree(from, pivot, depth + 1);
        node->children[1] = BuildTree(pivot, to, depth + 1);
    }
    return node;
}

void BVH::Traverse(Ray &ray, const std::shared_ptr<BVHNode>& node, int depth) {
    if(node->bbox->is_intersecting(ray)){

        if(node->is_leaf()){
            // intersect all triangles
            for(int i = node->span[0]; i < node->span[1]; i++){
                items_[i]->is_intersected(ray);
            }
        } else {
            Traverse(ray, node->children[0], depth + 1);
            Traverse(ray, node->children[1], depth + 1);
        }
    }
}

std::shared_ptr<BVHBbox> BVH::CalculateNodeBounds(int from, int to) {
    // // get the bounds of all vertices of all triangles in the current range
    std::shared_ptr<BVHBbox> bbox = items_[from]->get_bbox();
    for(int i = from + 1; i < to; i++){
        std::shared_ptr<BVHBbox> triangle_bbox = items_[i]->get_bbox();
        for(int j = 0; j < 3; j++){
            bbox->border_min[j] = min(bbox->border_min[j], triangle_bbox->border_min[j]);
            bbox->border_max[j] = max(bbox->border_max[j], triangle_bbox->border_max[j]);
        }
    }

    return bbox;
}

bool BVHBbox::is_intersecting(Ray &ray) {
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

void BVHTriangle::is_intersected(Ray &ray) {
    // Moller-Trumbore algorithm
    Vector3 ray_dir = ray.get_direction();
    Vector3 edge1 = vertices_[1].position - vertices_[0].position;
    Vector3 edge2 = vertices_[2].position - vertices_[0].position;
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
        if (t < ray.get_tfar()) {
            ray.bvh_hit_point->is_intersected = true;
            ray.bvh_hit_point->tfar = t;
            ray.bvh_hit_point->u = u;
            ray.bvh_hit_point->v = v;
            ray.bvh_hit_point->normal = get_normal(u, v);
            ray.bvh_hit_point->text_coords = get_coords(u, v);
            ray.bvh_hit_point->material = material;
        }
    }
}

void BVHTriangle::calculate_bbox() {
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

void BVHTriangle::calculate_area(){
    Vector3 u = vertices_[0].position - vertices_[1].position;
    Vector3 v = vertices_[0].position - vertices_[2].position;
    area = 0.5f * u.CrossProduct(v).L2Norm();
}

