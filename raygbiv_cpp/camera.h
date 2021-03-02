#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"
#include "ray.h"
#include "vec3.h"

class camera
{
  public:
      camera(): camera(point3(0,0,-1), point3(0,0,0), vec3(0,1,0), 40.0f, 16.0f/9.0f, 0.0f, 10.0f, 0.0f, 1.0f) {

    }
    camera(point3 lookfrom,
           point3 lookat,
           vec3 vup,
           float vfov, // vertical field-of-view in degrees
           float aspect_ratio,
           float aperture,
           float focus_dist,
           float _time0 = 0,
           float _time1 = 0)
    {
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2.0f);
        auto viewport_height = 2.0f * h;
        auto viewport_width = aspect_ratio * viewport_height;
        // our view faces -w
        // vup, v, and w are all in same plane

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        lower_left_corner = origin - horizontal / 2.0f - vertical / 2.0f - focus_dist * w;

        lens_radius = aperture / 2.0f;

        time0 = _time0;
        time1 = _time1;
    }

    ray get_ray(float s, float t) const
    {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();

        return ray(origin + offset,
                   lower_left_corner + s * horizontal + t * vertical - origin - offset,
                   random_float(time0, time1));
    }

  private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    float lens_radius;
    // shutter open/close times
    float time0;
    float time1;
};
#endif
