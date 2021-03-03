#include "scene.h"

#include "aarect.h"
#include "box.h"
#include "bvh_node.h"
#include "camera.h"
#include "constant_medium.h"
#include "material.h"
#include "moving_sphere.h"
#include "sphere.h"
#include "texture.h"

hittable_list
two_spheres()
{
    hittable_list objects;

    auto checker = make_shared<checker_texture>(color(0.2f, 0.3f, 0.1f), color(0.9f, 0.9f, 0.9f));

    objects.add(make_shared<sphere>(point3(0.0f, -10.0f, 0.0f), 10.0f, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(point3(0.0f, 10.0f, 0.0f), 10.0f, make_shared<lambertian>(checker)));

    return objects;
}

hittable_list
random_scene()
{
    hittable_list world;

    auto checker = make_shared<checker_texture>(color(0.2f, 0.3f, 0.1f), color(0.9f, 0.9f, 0.9f));
    world.add(make_shared<sphere>(point3(0.0f, -1000.0f, 0.0f), 1000.0f, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_float();
            point3 center(a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float());

            if ((center - point3(4, 0.2f, 0)).length() > 0.9f) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0.0f, random_float(0.0f, 0.5f), 0.0f);
                    world.add(make_shared<moving_sphere>(center, center2, 0.0f, 1.0f, 0.2f, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_float(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2f, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5f);
                    world.add(make_shared<sphere>(center, 0.2f, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5f);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0f, material1));

    auto material2 = make_shared<lambertian>(color(0.4f, 0.2f, 0.1f));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0f, material2));

    auto material3 = make_shared<metal>(color(0.7f, 0.6f, 0.5f), 0.0f);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0f, material3));

    return world;
}

hittable_list
two_perlin_spheres()
{
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4.0f);
    objects.add(make_shared<sphere>(point3(0.0f, -1000.0f, 0.0f), 1000.0f, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0.0f, 2.0f, 0.0f), 2.0f, make_shared<lambertian>(pertext)));

    return objects;
}

hittable_list
earth()
{
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2.0f, earth_surface);

    return hittable_list(globe);
}

hittable_list
simple_light()
{
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4.0f);
    objects.add(make_shared<sphere>(point3(0.0f, -1000.0f, 0.0f), 1000.0f, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0.0f, 2.0f, 0.0f), 2.0f, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
    objects.add(make_shared<xy_rect>(3.0f, 5.0f, 1.0f, 3.0f, -2.0f, difflight));

    objects.add(make_shared<sphere>(point3(0.0f, 7.0f, 0.0f), 2.0f, difflight));

    return objects;
}

hittable_list
cornell_box()
{
    hittable_list objects;

    auto red = make_shared<lambertian>(color(.65f, .05f, .05f));
    auto white = make_shared<lambertian>(color(.73f, .73f, .73f));
    auto green = make_shared<lambertian>(color(.12f, .45f, .15f));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, green));
    objects.add(make_shared<yz_rect>(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, red));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213.0f, 343.0f, 227.0f, 332.0f, 554.0f, light)));
    objects.add(make_shared<xz_rect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, white));
    objects.add(make_shared<xz_rect>(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, white));
    objects.add(make_shared<xy_rect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, white));

    shared_ptr<material> aluminum = make_shared<metal>(color(0.8f, 0.85f, 0.88f), 0.0f);
    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), aluminum);
    box1 = make_shared<rotate_y>(box1, 15.0f);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    objects.add(box1);

    auto glass = make_shared<dielectric>(1.5f);
    objects.add(make_shared<sphere>(point3(190, 90, 190), 90.0f, glass));

    // shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    // box2 = make_shared<rotate_y>(box2, -18.0f);
    // box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    // objects.add(box2);

    return objects;
}
#if 0
hittable_list cornell_box() {
    hittable_list objects;

    auto red = make_shared<lambertian>(color(.65f, .05f, .05f));
    auto white = make_shared<lambertian>(color(.73f, .73f, .73f));
    auto green = make_shared<lambertian>(color(.12f, .45f, .15f));
    auto light = make_shared<diffuse_light>(color(15.0f, 15.0f, 15.0f));

    objects.add(make_shared<yz_rect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, green));
    objects.add(make_shared<yz_rect>(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, red));
    objects.add(make_shared<xz_rect>(213.0f, 343.0f, 227.0f, 332.0f, 554.0f, light));
    objects.add(make_shared<xz_rect>(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, white));
    objects.add(make_shared<xz_rect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, white));
    objects.add(make_shared<xy_rect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, white));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0.0f, 0.0f, 0.0f), point3(165.0f, 330.0f, 165.0f), white);
    box1 = make_shared<rotate_y>(box1, 15.0f);
    box1 = make_shared<translate>(box1, vec3(265.0f, 0.0f, 295.0f));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(point3(0.0f, 0.0f, 0.0f), point3(165.0f, 165.0f, 165.0f), white);
    box2 = make_shared<rotate_y>(box2, -18.0f);
    box2 = make_shared<translate>(box2, vec3(130.0f, 0.0f, 65.0f));
    objects.add(box2);

    return objects;
}
#endif
hittable_list
cornell_smoke()
{
    hittable_list objects;

    auto red = make_shared<lambertian>(color(.65f, .05f, .05f));
    auto white = make_shared<lambertian>(color(.73f, .73f, .73f));
    auto green = make_shared<lambertian>(color(.12f, .45f, .15f));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    objects.add(make_shared<yz_rect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, green));
    objects.add(make_shared<yz_rect>(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, red));
    objects.add(make_shared<xz_rect>(113.0f, 443.0f, 127.0f, 432.0f, 554.0f, light));
    objects.add(make_shared<xz_rect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, white));
    objects.add(make_shared<xz_rect>(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, white));
    objects.add(make_shared<xy_rect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, white));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15.0f);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18.0f);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));

    objects.add(make_shared<constant_medium>(box1, 0.01f, color(0, 0, 0)));
    objects.add(make_shared<constant_medium>(box2, 0.01f, color(1, 1, 1)));

    return objects;
}

void
veach_mis(hittable_list& world, shared_ptr<hittable_list>& lights)
{
    auto mat1 = make_shared<lambertian>(color(1.0f, 0.0f, 0.0f));

    auto light1 = make_shared<sphere>(point3(-1.5, 3, -1), 0.03f, make_shared<diffuse_light>(color(15, 15, 15)));
    auto light2 = make_shared<sphere>(point3(-0.5, 3, -1), 0.1f, make_shared<diffuse_light>(color(15, 15, 15)));
    auto light3 = make_shared<sphere>(point3(0.5, 3, -1), 0.3f, make_shared<diffuse_light>(color(15, 15, 15)));
    auto light4 = make_shared<sphere>(point3(1.5, 3, -1), 0.5f, make_shared<diffuse_light>(color(15, 15, 15)));
    world.add(light1);
    world.add(light2);
    world.add(light3);
    world.add(light4);
    lights->add(light1);
    lights->add(light2);
    lights->add(light3);
    lights->add(light4);

    world.add(make_shared<translate>(
      make_shared<rotate_x>(make_shared<xy_rect>(-2.0f, 2.0f, -0.5f, 0.5f, 0.0f, mat1), 0.0f), vec3(0.0, 0.0, 0.0)));
    world.add(make_shared<translate>(
      make_shared<rotate_x>(make_shared<xy_rect>(-2.0f, 2.0f, -0.5f, 0.5f, 0.0f, mat1), -15.0f), vec3(0.0, -1.0, -0.2)));
    world.add(make_shared<translate>(
      make_shared<rotate_x>(make_shared<xy_rect>(-2.0f, 2.0f, -0.5f, 0.5f, 0.0f, mat1), -30.0f), vec3(0.0, -2.0, -0.6)));
    world.add(make_shared<translate>(
      make_shared<rotate_x>(make_shared<xy_rect>(-2.0f, 2.0f, -0.5f, 0.5f, 0.0f, mat1), -45.0f), vec3(0.0, -3.0, -1.2)));

    //    objects.add(make_shared<translate>(
  //      make_shared<rotate_y>(
    //        make_shared<flip_face>(
      //          make_shared<xz_rect>(-2.0f, 2.0f, -0.5f, 0.5f, 0.0f, mat1)), 45.0f), vec3(0.0, 0.0, 0.0)));

    //objects.add(make_shared<translate>(
      //make_shared<rotate_y>(make_shared<sphere>(point3(0,0,0), 1.0f, mat1), 45.0f), vec3(0.0, 0.0, 0.0)));
}

hittable_list
final_scene()
{
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48f, 0.83f, 0.53f));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0f;
            auto x0 = -1000.0f + i * w;
            auto z0 = -1000.0f + j * w;
            auto y0 = 0.0f;
            auto x1 = x0 + w;
            auto y1 = random_float(1.0f, 101.0f);
            auto z1 = z0 + w;

            boxes1.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    hittable_list objects;

    objects.add(make_shared<bvh_node>(boxes1, 0.0f, 1.0f));

    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    objects.add(make_shared<xz_rect>(123.0f, 423.0f, 147.0f, 412.0f, 554.0f, light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto moving_sphere_material = make_shared<lambertian>(color(0.7f, 0.3f, 0.1f));
    objects.add(make_shared<moving_sphere>(center1, center2, 0.0f, 1.0f, 50.0f, moving_sphere_material));

    objects.add(make_shared<sphere>(point3(260, 150, 45), 50.0f, make_shared<dielectric>(1.5f)));
    objects.add(make_shared<sphere>(point3(0, 150, 145), 50.0f, make_shared<metal>(color(0.8f, 0.8f, 0.9f), 1.0f)));

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70.0f, make_shared<dielectric>(1.5f));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.2f, color(0.2f, 0.4f, 0.9f)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000.0f, make_shared<dielectric>(1.5f));
    objects.add(make_shared<constant_medium>(boundary, .0001f, color(1, 1, 1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    objects.add(make_shared<sphere>(point3(400, 200, 400), 100.0f, emat));
    auto pertext = make_shared<noise_texture>(0.1f);
    objects.add(make_shared<sphere>(point3(220, 280, 300), 80.0f, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73f, .73f, .73f));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0, 165), 10.0f, white));
    }

    objects.add(make_shared<translate>(make_shared<rotate_y>(make_shared<bvh_node>(boxes2, 0.0f, 1.0f), 15.0f),
                                       vec3(-100, 270, 395)));

    return objects;

    // hittable_list worldbvh;
    // worldbvh.add(make_shared<bvh_node>(objects, 0.0f, 1.0f));

    // return worldbvh;
}

void
load_scene(int scenetype, render_settings& rs, hittable_list& world, shared_ptr<hittable_list>& lights, camera& cam, color& background)
{
    auto aspect_ratio = 16.0f / 9.0f;

    point3 lookfrom;
    point3 lookat;
    auto vfov = 40.0f;
    auto aperture = 0.0f;

    rs.image_width = 400;
    rs.samples_per_pixel = 100;
    rs.max_path_size = 50;
    switch (scenetype) {
        case 1:
            world = random_scene();
            lookfrom = point3(13.0f, 2.0f, 3.0f);
            lookat = point3(0.0f, 0.0f, 0.0f);
            vfov = 20.0f;
            aperture = 0.1f;
            background = color(0.70f, 0.80f, 1.00f);
            break;

        case 2:
            world = two_spheres();
            lookfrom = point3(13.0f, 2.0f, 3.0f);
            lookat = point3(0.0f, 0.0f, 0.0f);
            vfov = 20.0f;
            background = color(0.70f, 0.80f, 1.00f);
            break;
        case 3:
            world = two_perlin_spheres();
            lookfrom = point3(13.0f, 2.0f, 3.0f);
            lookat = point3(0.0f, 0.0f, 0.0f);
            vfov = 20.0f;
            background = color(0.70f, 0.80f, 1.00f);
            break;
        case 4:
            world = earth();
            lookfrom = point3(13.0f, 2.0f, 3.0f);
            lookat = point3(0.0f, 0.0f, 0.0f);
            vfov = 20.0f;
            background = color(0.70f, 0.80f, 1.00f);
            break;
        case 5:
            world = simple_light();
            rs.samples_per_pixel = 400;
            background = color(0.0f, 0.0f, 0.0f);
            lookfrom = point3(26.0f, 3.0f, 6.0f);
            lookat = point3(0.0f, 2.0f, 0.0f);
            vfov = 20.0f;
            break;
        case 6:
            world = cornell_box();
            aspect_ratio = 1.0f;
            rs.image_width = 600;
            rs.samples_per_pixel = 1000;
            background = color(0.0f, 0.0f, 0.0f);
            lookfrom = point3(278.0f, 278.0f, -800.0f);
            lookat = point3(278.0f, 278.0f, 0.0f);
            vfov = 40.0f;
            // lights =
            //  make_shared<xz_rect>(213.0f, 343.0f, 227.0f, 332.0f, 554.0f, shared_ptr<material>());

            // lights =
            //   make_shared<sphere>(point3(190, 90, 190), 90.0f, shared_ptr<material>());


            lights->add(make_shared<xz_rect>(213.0f, 343.0f, 227.0f, 332.0f, 554.0f, shared_ptr<material>()));
            lights->add(make_shared<sphere>(point3(190, 90, 190), 90.0f, shared_ptr<material>()));
            break;
        case 7:
            world = cornell_smoke();
            aspect_ratio = 1.0f;
            rs.image_width = 600;
            rs.samples_per_pixel = 200;
            lookfrom = point3(278.0f, 278.0f, -800.0f);
            lookat = point3(278.0f, 278.0f, 0.0f);
            vfov = 40.0f;
            break;
        case 8: 
            {
            veach_mis(world, lights);
            aspect_ratio = 1.0f;
            rs.image_width = 600;
            rs.samples_per_pixel = 10;
            background = color(0.0f, 0.0f, 1.0f);
            lookfrom = point3(0.0f, 0.0f, -10.0f);
            lookat = point3(0.0f, 0.0f, 0.0f);
            vfov = 40.0f;
            }
            break;

        default:
        case 9:
            world = final_scene();
            aspect_ratio = 1.0f;
            rs.image_width = 800;
            rs.samples_per_pixel = 10000;
            background = color(0, 0, 0);
            lookfrom = point3(478, 278, -600);
            lookat = point3(278, 278, 0);
            vfov = 40.0f;
            break;
    }
    vec3 vup(0.0f, 1.0f, 0.0f);
    auto dist_to_focus = 10.0f;

    // finalize the image dimensions
    rs.setWidthAndAspect(rs.image_width, aspect_ratio);

    auto time0 = 0.0f;
    auto time1 = 1.0f;

    cam = camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, time0, time1);
}