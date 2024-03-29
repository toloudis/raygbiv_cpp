#pragma once

#ifndef VEC3_H
#define VEC3_H

#include "rtweekend.h"

#include <cmath>
#include <iostream>

using std::sqrt;

using glm::vec3;
// class vec3
// {
//   public:
//     vec3()
//       : e{ 0, 0, 0 }
//     {}
//     vec3(float e0, float e1, float e2)
//       : e{ e0, e1, e2 }
//     {}

//     float x() const { return e[0]; }
//     float y() const { return e[1]; }
//     float z() const { return e[2]; }

//     bool near_zero() const
//     {
//         // Return true if the vector is close to zero in all dimensions.
//         const auto s = 1e-8;
//         return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
//     }

//     vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
//     float operator[](int i) const { return e[i]; }
//     float& operator[](int i) { return e[i]; }

//     vec3& operator+=(const vec3& v)
//     {
//         e[0] += v[0];
//         e[1] += v[1];
//         e[2] += v[2];
//         return *this;
//     }

//     vec3& operator*=(const float t)
//     {
//         e[0] *= t;
//         e[1] *= t;
//         e[2] *= t;
//         return *this;
//     }
//     vec3& operator*=(const vec3 t)
//     {
//         e[0] *= t[0];
//         e[1] *= t[1];
//         e[2] *= t[2];
//         return *this;
//     }

//     vec3& operator/=(const float t) { return *this *= 1 / t; }

//     float length() const { return sqrt(length_squared()); }

//     float length_squared() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }

//     inline static vec3 random() { return vec3(random_float(), random_float(), random_float()); }

//     inline static vec3 random(float min, float max)
//     {
//         return vec3(random_float(min, max), random_float(min, max), random_float(min, max));
//     }

//   public:
//     float e[3];
// };

// Type aliases for vec3
using point3 = vec3; // 3D point
using color = vec3;  // RGB color

// vec3 Utility Functions

inline std::ostream&
operator<<(std::ostream& out, const vec3& v)
{
    return out << v[0] << ' ' << v[1] << ' ' << v[2];
}

inline vec3
operator+(const vec3& u, const vec3& v)
{
    return vec3(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
}

inline vec3
operator-(const vec3& u, const vec3& v)
{
    return vec3(u[0] - v[0], u[1] - v[1], u[2] - v[2]);
}

inline vec3
operator*(const vec3& u, const vec3& v)
{
    return vec3(u[0] * v[0], u[1] * v[1], u[2] * v[2]);
}

inline vec3
operator*(float t, const vec3& v)
{
    return vec3(t * v[0], t * v[1], t * v[2]);
}

inline vec3
operator*(const vec3& v, float t)
{
    return t * v;
}

inline vec3
operator/(vec3 v, float t)
{
    return (1 / t) * v;
}

inline float
dot(const vec3& u, const vec3& v)
{
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

inline vec3
cross(const vec3& u, const vec3& v)
{
    return vec3(
      u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2], u[0] * v[1] - u[1] * v[0]);
}

inline vec3
unit_vector(vec3 v)
{
    return v / v.length();
}

inline vec3
random_in_unit_sphere()
{
    while (true) {
        auto p = glm::linearRand(vec3(-1,-1,-1), vec3(1,1,1));//vec3::random(-1.0f, 1.0f);
        if (glm::length2(p) >= 1.0f)
            continue;
        return p;
    }
}

inline vec3
random_unit_vector()
{
    return unit_vector(random_in_unit_sphere());
}

inline vec3
random_in_hemisphere(const vec3& normal)
{
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0f) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

inline vec3
random_in_unit_disk()
{
    while (true) {
        auto p = vec3(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f), 0.0f);
        if (glm::length2(p) >= 1.0f)
            continue;
        return p;
    }
}

inline vec3
reflect(const vec3& v, const vec3& n)
{
    return v - 2.0f * dot(v, n) * n;
}

inline vec3
refract(const vec3& uv, const vec3& n, float etai_over_etat)
{
    auto cos_theta = fmin(dot(-uv, n), 1.0f);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -sqrt(fabs(1.0f - glm::length2(r_out_perp))) * n;
    return r_out_perp + r_out_parallel;
}

inline vec3
random_cosine_direction()
{
    auto r1 = random_float();
    auto r2 = random_float();
    auto z = sqrt(1 - r2);

    auto phi = 2 * pi * r1;
    auto x = cos(phi) * sqrt(r2);
    auto y = sin(phi) * sqrt(r2);

    return vec3(x, y, z);
}

inline vec3
random_to_sphere(float radius, float distance_squared)
{
    auto r1 = random_float();
    auto r2 = random_float();
    auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

    auto phi = 2 * pi * r1;
    auto x = cos(phi) * sqrt(1 - z * z);
    auto y = sin(phi) * sqrt(1 - z * z);

    return vec3(x, y, z);
}

#endif
