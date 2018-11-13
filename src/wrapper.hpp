#ifndef _WRAPPER_H_
#define _WRAPPER_H_

#define STB_IMAGE_IMPLEMENTATION
#include <string>
#include "deps/stb_image.h"
#include "utility.hpp"
#include "vec.hpp"

void load_rgb_image_file(const std::string &file_path, Image &image) {
    int x, y, n;
    unsigned char *data = stbi_load(file_path.c_str(), &x, &y, &n, 0);

    image.set_image_size(x, y);
    int ch = 3;
    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
            double r = *(data + (ch * (i * x + j))) / 256.0;
            double g = *(data + (ch * (i * x + j) + 1)) / 256.0;
            double b = *(data + (ch * (i * x + j) + 2)) / 256.0;
            image.set_color(Color(r, g, b), i, j);
        }
    }
}

#endif
