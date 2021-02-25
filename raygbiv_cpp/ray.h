#pragma once

#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray
{
  public:
    ray()
      : orig()
      , dir()
      , tm(0)
    {}

    ray(const point3& origin,
        const vec3& direction,
        float time = 0.0f)
      : orig(origin)
      , dir(direction)
      , tm(time)
    {}

    point3 origin() const { return orig; }
    vec3 direction() const { return dir; }
    float time() const { return tm; }

    point3 at(float t) const { return orig + t * dir; }

  public:
    point3 orig;
    vec3 dir;
    float tm;
};

#endif
