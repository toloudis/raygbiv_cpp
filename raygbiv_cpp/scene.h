#pragma once

#ifndef SCENE_H
#define SCENE_H

#include "hittable_list.h"

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
