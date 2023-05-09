#include "image_buffer.h"

#include "vec3.h"

imageBuffer::imageBuffer(int w, int h): w(w), h(h) {
    data = new uint8_t[w * h * 3];
}

void imageBuffer::putPixel(int samples_per_pixel, color& pixel_color, int i, int j)
{
    auto r = pixel_color.x;
    auto g = pixel_color.y;
    auto b = pixel_color.z;

    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0f / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // Write the translated [0,255] value of each color component.
    int ir = static_cast<int>(256 * clamp(r, 0.0f, 0.999f));
    int ig = static_cast<int>(256 * clamp(g, 0.0f, 0.999f));
    int ib = static_cast<int>(256 * clamp(b, 0.0f, 0.999f));

    data[0 + i * 3 + j * (w * 3)] = (uint8_t)ir;
    data[1 + i * 3 + j * (w * 3)] = (uint8_t)ig;
    data[2 + i * 3 + j * (w * 3)] = (uint8_t)ib;
}
