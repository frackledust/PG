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
