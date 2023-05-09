// montecarlo.cpp : Defines the entry point for the application.
//

#include "montecarlo.h"

#include "rtweekend.h"
#include "vec3.h"

#include <iomanip>
#include <iostream>

#include <iomanip>
#include <iostream>
#include <math.h>
#include <stdlib.h>

float domain_min = 0.0f;
float domain_max = 2.0f;
float
f(float x)
{
    return x * x;
}

// to be a pdf, it must be always non-negative and integrate to 1 over its domain
inline float
pdf(float x)
{
    return 0.5f * x;
}
// inverse of the cdf P(x) where
// cdf P(x) is integral of pdf from -inf up to x.
// therefore PDF p(x) = derivative of P(x): dP(x)/dx
// P(X) is the Probability that a variable from the pdf distribution is <= X.
// P(x in [a,b]) = integral from a to b of p(x)dx
// the param passed in is a uniform random.
// this function is used to select a random number with distribution pdf.
inline float
Pinv(float x)
{
    return sqrt(4.0f * x);
}

vec3
uniformSampleHemisphere(float u, float v, float* pdf = nullptr)
{
    auto z = u;
    auto r = std::sqrt(std::max(0.0f, 1.0f - z * z));
    auto phi = 2 * pi * v;
    if (pdf) {
        *pdf = 1.0f / (2.0f * pi);
    }
    return vec3(r * std::cos(phi), r * std::sin(phi), z);
}
vec3
uniformSampleSphere(float u, float v, float* pdf = nullptr)
{
    auto z = 1 - 2 * u;
    auto r = std::sqrt(std::max(0.0f, 1.0f - z * z));
    auto phi = 2 * pi * v;
    if (pdf) {
        *pdf = 1.0f / (4.0f * pi);
    }
    return vec3(r * std::cos(phi), r * std::sin(phi), z);
}
vec3
uniformSampleDisk(float u, float v, float* pdf = nullptr)
{
    auto r = std::sqrt(u);
    auto theta = 2 * pi * v;
    if (pdf) {
        *pdf = 1.0f / pi;
    }
    return vec3(r * std::cos(theta), r * std::sin(theta), 0);
}
vec3
concentricSampleDisk(float u, float v, float* pdf = nullptr)
{
    if (pdf) {
        *pdf = 1.0f / pi;
    }
    auto uOffset = 2.f * u - 1.0f;
    auto vOffset = 2.f * v - 1.0f;
    if (uOffset == 0 && vOffset == 0)
        return vec3(0, 0, 0);

    float theta, r;
    if (std::abs(uOffset) > std::abs(vOffset)) {
        r = uOffset;
        theta = (pi / 4.0f) * (vOffset / uOffset);
    } else {
        r = vOffset;
        theta = (pi / 2.0f) - (pi / 4.0f) * (uOffset / vOffset);
    }
    return r * vec3(std::cos(theta), std::sin(theta), 0.0f);
}

vec3
cosineSampleHemisphere(float u, float v, float* pdf = nullptr)
{
    vec3 d = concentricSampleDisk(u, v);
    d[2] = std::sqrt(std::max(0.0f, 1 - d.x * d.x - d.y * d.y));
    if (pdf) {
        // TODO : this should be costheta / pi.  confirm that d.z = cos theta
        *pdf = d.z / pi;
    }
    return d;
}

vec3
uniformSampleCone(float u, float v, float cosThetaMax, float* pdf = nullptr)
{
    if (pdf) {
        *pdf = 1.0f / (2.0f * pi * (1.0f - cosThetaMax));
    }
    auto cosTheta = (1.0f - u) + u * cosThetaMax;
    auto sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
    auto phi = v * 2 * pi;
    return vec3(std::cos(phi) * sinTheta, std::sin(phi) * sinTheta, cosTheta);
}
// returns barycentric coordinates
vec3
uniformSampleTriangle(float u, float v, float area, float* pdf = nullptr)
{
    if (pdf) {
        *pdf = 1.0f / area;
    }
    auto su0 = std::sqrt(u);
    return vec3(1.0f - su0, v * su0, 0.0f);
}

int
main()
{
    int N = 1000000;
    auto sum = 0.0f;
    for (int i = 0; i < N; i++) {
        auto x = Pinv(random_float());
        sum += f(x) / pdf(x);
    }
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << sum / N << '\n';
}
