#include "sphere.h"

#include "onb.h"

bool
sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
    vec3 oc = r.origin() - center;
    auto a = glm::length2(r.direction());
    auto half_b = dot(oc, r.direction());
    auto c = glm::length2(oc) - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0)
        return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat_ptr = mat_ptr;

    return true;
}

bool
sphere::bounding_box(float time0, float time1, aabb& output_box) const
{
    output_box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
    return true;
}

float
sphere::pdf_value(const point3& o, const vec3& v) const
{
    hit_record rec;
    if (!this->hit(ray(o, v), RAY_EPSILON, infinity, rec))
        return 0;

    auto cos_theta_max = sqrt(1 - radius * radius / glm::length2(center - o));
    auto solid_angle = 2 * pi * (1 - cos_theta_max);

    return 1 / solid_angle;
}

vec3
sphere::random(const point3& o) const
{
    vec3 direction = center - o;
    auto distance_squared = glm::length2(direction);
    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local(random_to_sphere(radius, distance_squared));
}
