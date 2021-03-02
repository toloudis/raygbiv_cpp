#pragma once

#ifndef AARECT_H
#define AARECT_H

#include "rtweekend.h"

#include "hittable.h"

class xy_rect : public hittable
{
  public:
    xy_rect()
      : x0(0)
      , x1(0)
      , y0(0)
      , y1(0)
      , k(0)
    {}

    xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, shared_ptr<material> mat)
      : x0(_x0)
      , x1(_x1)
      , y0(_y0)
      , y1(_y1)
      , k(_k)
      , mp(mat){};

    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override
    {
        // The bounding box must have non-zero width in each dimension, so pad the Z
        // dimension a small amount.
        output_box = aabb(point3(x0, y0, k - 0.0001f), point3(x1, y1, k + 0.0001f));
        return true;
    }

    virtual float pdf_value(const point3& origin, const vec3& v) const override
    {
        hit_record rec;
        if (!this->hit(ray(origin, v), RAY_EPSILON, infinity, rec))
            return 0;

        auto area = (x1 - x0) * (y1 - y0);
        auto distance_squared = rec.t * rec.t * v.length_squared();
        auto cosine = fabs(dot(v, rec.normal) / v.length());

        return distance_squared / (cosine * area);
    }

    virtual vec3 random(const point3& origin) const override
    {
        auto random_point = point3(random_float(x0, x1), random_float(y0, y1), k);
        return random_point - origin;
    }

  public:
    shared_ptr<material> mp;
    float x0, x1, y0, y1, k;
};


class xz_rect : public hittable
{
  public:
    xz_rect()
      : x0(0)
      , x1(0)
      , z0(0)
      , z1(0)
      , k(0)
    {}

    xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, shared_ptr<material> mat)
      : x0(_x0)
      , x1(_x1)
      , z0(_z0)
      , z1(_z1)
      , k(_k)
      , mp(mat){};

    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override
    {
        // The bounding box must have non-zero width in each dimension, so pad the Y
        // dimension a small amount.
        output_box = aabb(point3(x0, k - 0.0001f, z0), point3(x1, k + 0.0001f, z1));
        return true;
    }

    virtual float pdf_value(const point3& origin, const vec3& v) const override
    {
        hit_record rec;
        if (!this->hit(ray(origin, v), RAY_EPSILON, infinity, rec))
            return 0;

        auto area = (x1 - x0) * (z1 - z0);
        auto distance_squared = rec.t * rec.t * v.length_squared();
        auto cosine = fabs(dot(v, rec.normal) / v.length());

        return distance_squared / (cosine * area);
    }

    virtual vec3 random(const point3& origin) const override
    {
        auto random_point = point3(random_float(x0, x1), k, random_float(z0, z1));
        return random_point - origin;
    }

  public:
    shared_ptr<material> mp;
    float x0, x1, z0, z1, k;
};

class yz_rect : public hittable
{
  public:
    yz_rect()
      : y0(0)
      , y1(0)
      , z0(0)
      , z1(0)
      , k(0)
    {}

    yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, shared_ptr<material> mat)
      : y0(_y0)
      , y1(_y1)
      , z0(_z0)
      , z1(_z1)
      , k(_k)
      , mp(mat){};

    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override
    {
        // The bounding box must have non-zero width in each dimension, so pad the X
        // dimension a small amount.
        output_box = aabb(point3(k - 0.0001f, y0, z0), point3(k + 0.0001f, y1, z1));
        return true;
    }

    virtual float pdf_value(const point3& origin, const vec3& v) const override
    {
        hit_record rec;
        if (!this->hit(ray(origin, v), RAY_EPSILON, infinity, rec))
            return 0;

        auto area = (z1 - z0) * (y1 - y0);
        auto distance_squared = rec.t * rec.t * v.length_squared();
        auto cosine = fabs(dot(v, rec.normal) / v.length());

        return distance_squared / (cosine * area);
    }

    virtual vec3 random(const point3& origin) const override
    {
        auto random_point = point3(k, random_float(y0, y1), random_float(z0, z1));
        return random_point - origin;
    }

  public:
    shared_ptr<material> mp;
    float y0, y1, z0, z1, k;
};
#endif
