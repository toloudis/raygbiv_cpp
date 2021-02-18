#pragma once

#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385f;

const float RAY_EPSILON = 0.001f;

// Utility Functions

inline float degrees_to_radians(float degrees) {
    return degrees * pi / 180.0f;
}

inline float random_float() {
    static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    static std::mt19937 generator;
    return distribution(generator);
}

inline float random_float(float min, float max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_float();
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_float(static_cast<float>(min), static_cast<float>(max) + 1.0f));
}


inline float clamp(float x, float min, float max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
// Common Headers

#include "ray.h"
#include "vec3.h"

inline vec3 random_cosine_direction() {
    auto r1 = random_float();
    auto r2 = random_float();
    auto z = sqrt(1 - r2);

    auto phi = 2 * pi * r1;
    auto x = cos(phi) * sqrt(r2);
    auto y = sin(phi) * sqrt(r2);

    return vec3(x, y, z);
}

#endif
