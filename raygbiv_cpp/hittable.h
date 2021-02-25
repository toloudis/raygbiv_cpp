#pragma once

#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.h"
#include "rtweekend.h"

class material;

struct hit_record
{
    point3 p;
    vec3 normal;
    shared_ptr<material> mat_ptr;
    float t;
    float u;
    float v;
    bool front_face;

    hit_record()
      : t(0)
      , u(0)
      , v(0)
      , front_face(true)
    {}

    inline void set_face_normal(const ray& r, const vec3& outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
  public:
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(float time0, float time1, aabb& output_box) const = 0;

    // evaluate the pdf of the ray from o toward v
    virtual float pdf_value(const point3& o, const vec3& v) const { return 0.0; }

    // return a direction from o to a (uniform?) random point on the hittable
    virtual vec3 random(const vec3& o) const { return vec3(1, 0, 0); }
};

class translate : public hittable
{
  public:
    translate(shared_ptr<hittable> p, const vec3& displacement)
      : ptr(p)
      , offset(displacement)
    {}

    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override;

  public:
    shared_ptr<hittable> ptr;
    vec3 offset;
};


class rotate_y : public hittable
{
  public:
    rotate_y(shared_ptr<hittable> p, float angle);

    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override
    {
        output_box = bbox;
        return hasbox;
    }

  public:
    shared_ptr<hittable> ptr;
    float sin_theta;
    float cos_theta;
    bool hasbox;
    aabb bbox;
};


class flip_face : public hittable
{
  public:
    flip_face(shared_ptr<hittable> p)
      : ptr(p)
    {}

    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override
    {

        if (!ptr->hit(r, t_min, t_max, rec))
            return false;

        rec.front_face = !rec.front_face;
        return true;
    }

    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override
    {
        return ptr->bounding_box(time0, time1, output_box);
    }

  public:
    shared_ptr<hittable> ptr;
};

#endif
