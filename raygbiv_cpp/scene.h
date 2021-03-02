#pragma once

#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "hittable_list.h"

struct render_settings
{
    int image_width = 1;
    int image_height = 1;
    int samples_per_pixel = 1;
    int max_path_size = 1;

    void setWidthAndAspect(int width, float aspect)
    {
        image_width = width;
        image_height = static_cast<int>(width / aspect);
    }
};

void
load_scene(int scenetype, render_settings& rs, hittable_list& world, shared_ptr<hittable>& lights, camera& cam, color& background);

hittable_list
two_spheres();

hittable_list
random_scene();

hittable_list
two_perlin_spheres();

hittable_list
earth();

hittable_list
simple_light();

hittable_list
cornell_box();

hittable_list
cornell_smoke();

hittable_list
final_scene();

#endif
