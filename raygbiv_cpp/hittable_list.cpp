#include "hittable_list.h"

bool
hittable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = t_max;

    for (const auto& object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

bool
hittable_list::bounding_box(float time0, float time1, aabb& output_box) const
{
    if (objects.empty())
        return false;

    aabb temp_box;
    bool first_box = true;

    for (const auto& object : objects) {
        if (!object->bounding_box(time0, time1, temp_box))
            return false;
        output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
        first_box = false;
    }

    return true;
}

float
hittable_list::pdf_value(const point3& o, const vec3& v) const
{
    if (objects.size() == 0) {
        // this has got to be some kind of error
        return 0.0f;
    }

    // uniform mixture (could these weights be proportional to the solid angle subtended? do the underlying pdfs account
    // for that?)
    auto weight = 1.0f / objects.size();
    auto sum = 0.0f;

    for (const auto& object : objects)
        sum += weight * object->pdf_value(o, v);

    return sum;
}

vec3
hittable_list::random(const vec3& o) const
{
    // choose random object, then generate random point on object?
    auto int_size = static_cast<int>(objects.size());
    if (int_size == 0) {
        // this has got to be some kind of error.
        return vec3(0.0f, 0.0f, 1.0f);
    }
    auto index = random_int(0, int_size - 1);
    return objects[index]->random(o);
}
