#pragma once

#include "vec3.h"

#include <inttypes.h>

class imageBuffer
{
  public:
    imageBuffer(int w, int h);
    void putPixel(int samples_per_pixel, color& pixel_color, int i, int j);

    int w, h;
    uint8_t* data;
};