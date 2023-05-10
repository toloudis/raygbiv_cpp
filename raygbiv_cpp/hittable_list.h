#pragma once

#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr;

class hittable_list : public hittable
{
  public:
    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    size_t size() { return objects.size(); }
    void clear() { objects.clear(); }
    void add(shared_ptr<hittable> object) { objects.push_back(object); }

    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override;
    virtual float pdf_value(const point3& o, const vec3& v) const override;
    virtual vec3 random(const vec3& o) const override;

  public:
    std::vector<shared_ptr<hittable>> objects;
};

#endif
