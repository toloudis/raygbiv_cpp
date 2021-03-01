// raygbiv_cpp.cpp : Defines the entry point for the application.
//

#include "raygbiv_cpp.h"

#include "rtweekend.h"

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "image_buffer.h"
#include "material.h"
#include "pdf.h"
#include "scene.h"
#include "threadpool.h"

//TODO remove these
#include "aarect.h"
#include "sphere.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <vector>

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


color
ray_color(const ray& r, const color& background, const hittable& world, shared_ptr<hittable>& lights, int depth)
{
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0.0f, 0.0f, 0.0f);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, RAY_EPSILON, infinity, rec))
        return background;

    scatter_record srec;
    color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);
    // returns the scattering pdf for this material inside of srec
    if (!rec.mat_ptr->scatter(r, rec, srec))
        return emitted;

    // implicitly sampled specular ray
    if (srec.is_specular) {
        return srec.attenuation * ray_color(srec.specular_ray, background, world, lights, depth - 1);
    }

    auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);

    // 50-50 chance of sampling toward light or toward scatter direction
    mixture_pdf p(light_ptr, srec.pdf_ptr);

    // generate sample from MIS pdf
    ray scattered = ray(rec.p, p.generate(), r.time());
    // evaluate pdf(generated sample)
    auto pdf_val = p.value(scattered.direction());

    return emitted + srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) *
                       ray_color(scattered, background, world, lights, depth - 1) / pdf_val;
}

color
path_color(const ray& r, const color& background, const hittable& world, shared_ptr<hittable>& lights, int depth)
{
    // this is the running total color sample for this path
    color path_contrib = color(0.0f, 0.0f, 0.0f);

    // attenuation must be multiplied again on each path segment
    // so that it decreases for longer paths
    color attenuation = color(1.0f, 1.0f, 1.0f);

    // this is the ray for the current path segment
    ray path_ray = r;

    for (int i = 0; i < depth; ++i) {
        hit_record rec;
        // do intersection test
        bool hit = world.hit(path_ray, RAY_EPSILON, infinity, rec);

        // If the ray hits nothing, add background color contribution and terminate path
        if (!hit) {
            path_contrib += attenuation * background;
            break;
        }

        scatter_record srec;
        color emitted = rec.mat_ptr->emitted(path_ray, rec, rec.u, rec.v, rec.p);
        // returns the scattering pdf for this material inside of srec
        // if scatter returns false, terminate path! 
        if (!rec.mat_ptr->scatter(path_ray, rec, srec)) {
            path_contrib += attenuation * emitted;
            break;
        }

        if (srec.is_specular) {
            // implicitly sampled specular ray
            // pdf is delta function for pure specular, and the relevant factors = 1 (?)
            attenuation *= srec.attenuation;
            // set the next ray to trace
            path_ray = srec.specular_ray;
        }
        else {
            auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);

            // 50-50 chance of sampling toward light or toward scatter direction
            mixture_pdf p(light_ptr, srec.pdf_ptr);

            // generate sample from MIS pdf: this will be the next path segment to trace
            ray scattered = ray(rec.p, p.generate(), r.time());
            // evaluate pdf(generated sample)
            auto pdf_val = p.value(scattered.direction());

            path_contrib += attenuation * emitted;
            attenuation *= srec.attenuation * rec.mat_ptr->scattering_pdf(path_ray, rec, scattered) / pdf_val;
            path_ray = scattered;
        }

    }
    return path_contrib;
}

bool
render_tile(const hittable_list& world,
            shared_ptr<hittable>& lights,
            const camera& cam,
            imageBuffer* image,
            const render_settings& rs,
            color background,
            int xoffset,
            int yoffset,
            int tilewidth,
            int tileheight)
{
    int ystart = yoffset;
    int yend = yoffset + tileheight;
    int xstart = xoffset;
    int xend = xoffset + tilewidth;

    std::stringstream stream; // #include <sstream> for this
    stream << "Start tile " << xstart << "," << ystart << "-" << xend << "," << yend << std::endl;
    std::cerr << stream.str();

    for (int j = yend - 1; j >= ystart; --j) {
        for (int i = xstart; i < xend; ++i) {

            color pixel_color(0.0f, 0.0f, 0.0f);
            for (int s = 0; s < rs.samples_per_pixel; ++s) {
                auto u = (i + random_float()) / (rs.image_width - 1);
                auto v = (j + random_float()) / (rs.image_height - 1);
                ray r = cam.get_ray(u, v);
                //pixel_color += ray_color(r, background, world, lights, rs.max_path_size);
                pixel_color += path_color(r, background, world, lights, rs.max_path_size);
            }

            image->putPixel(rs.samples_per_pixel, pixel_color, i, j);
        }
    }

    std::stringstream stream2; // #include <sstream> for this
    stream2 << "End tile " << xstart << "," << ystart << "-" << xend << "," << yend << std::endl;
    std::cerr << stream2.str();

    return true;
}

int
main()
{

    // Image
    render_settings rs;
    rs.image_width = 400;
    rs.samples_per_pixel = 100;
    rs.max_path_size = 50;

    auto aspect_ratio = 16.0f / 9.0f;

    // World

    hittable_list world;

    point3 lookfrom;
    point3 lookat;
    auto vfov = 40.0f;
    auto aperture = 0.0f;
    color background(0, 0, 0);

    shared_ptr<hittable> lights;

    switch (6) {
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

            lights = make_shared<hittable_list>();
            ((hittable_list*)lights.get())
              ->add(make_shared<xz_rect>(213.0f, 343.0f, 227.0f, 332.0f, 554.0f, shared_ptr<material>()));
            ((hittable_list*)lights.get())
              ->add(make_shared<sphere>(point3(190, 90, 190), 90.0f, shared_ptr<material>()));
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
        default:
        case 8:
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

    // Camera

    vec3 vup(0.0f, 1.0f, 0.0f);
    auto dist_to_focus = 10.0f;

    // finalize the image dimensions
    rs.setWidthAndAspect(rs.image_width, aspect_ratio);

    //uint8_t* image = new uint8_t[rs.image_width * rs.image_height * 3];
    imageBuffer* image = new imageBuffer(rs.image_width, rs.image_height);

    auto time0 = 0.0f;
    auto time1 = 1.0f;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, time0, time1);

    // Render
    auto start = std::chrono::high_resolution_clock::now();

    unsigned int number_of_cores = std::thread::hardware_concurrency();

    std::vector<std::future<bool>> jobs;
    raygbiv::Tasks tasks;
    // divide image into a set of tiles to run per thread.
    int n_x_tiles = 4;
    int n_y_tiles = 4;
    for (int i = 0; i < n_y_tiles; ++i) {
        for (int j = 0; j < n_x_tiles; ++j) {

            int xoffset = j * rs.image_width / n_x_tiles;
            int yoffset = i * rs.image_height / n_y_tiles;
            int tilewidth = rs.image_width / n_x_tiles;
            int tileheight = rs.image_height / n_y_tiles;
            if (xoffset + tilewidth > rs.image_width) {
                tilewidth = rs.image_width - xoffset;
            }
            if (yoffset + tileheight > rs.image_height) {
                tileheight = rs.image_height - yoffset;
            }
            jobs.push_back(tasks.queue([&world,
                                        &lights,
                                        &cam,
                                        image,
                                        &rs,
                                        background,
                                        xoffset,
                                        yoffset,
                                        tilewidth,
                                        tileheight]() -> bool {
                return render_tile(world,
                                   lights,
                                   cam,
                                   image,
                                   rs,
                                   background,
                                   xoffset,
                                   yoffset,
                                   tilewidth,
                                   tileheight);
            }));
        }
    }
    tasks.start(number_of_cores);
    for_each(jobs.begin(), jobs.end(), [](auto& x) { x.get(); });

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cerr << "\nRender duration = " << duration.count() / 1000.f << " s" << std::endl;
    std::cerr << "\nRender Done.\n";

    stbi_flip_vertically_on_write(1);
    stbi_write_png("out.png", rs.image_width, rs.image_height, 3, image->data, 3 * rs.image_width);

    std::cerr << "\nDone.\n";
}
