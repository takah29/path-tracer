#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "vec.h"

struct Image {
    int width_res, height_res;
    std::vector<Color> data;

    Image(const int &width_res, const int &height_res)
        : width_res(width_res), height_res(height_res), data(height_res * width_res) {}

    inline Color get_color(const int &row, const int &column) const {
        return data[row * height_res + column];
    }
};

inline double clamp(const double x, const double min = 0.0, const double max = 1.0) {
    if (x > max) {
        return max;
    } else if (x < min) {
        return min;
    } else {
        return x;
    }
}

inline int to_int(const double x) { return static_cast<int>(pow(clamp(x), 1 / 2.2) * 255.0 + 0.5); }

void save_ppm_file(const std::string &filename, const std::vector<Color> &image, const int width,
                   const int height) {
    std::ofstream outfile(filename, std::ios_base::out);

    outfile << "P3" << std::endl;
    outfile << width << " " << height << std::endl;
    outfile << "255" << std::endl;

    for (int i = 0; i < width * height; i++) {
        outfile << to_int(image[i].x) << " " << to_int(image[i].y) << " " << to_int(image[i].z)
                << std::endl;
    }

    outfile.close();
}

std::vector<std::string> split(const std::string &s, const char delim) {
    std::vector<std::string> result;
    std::string elem;

    for (const auto &c : s) {
        if (c != delim) {
            elem += c;
        } else {
            result.push_back(elem);
            elem.clear();
        }
    }

    result.push_back(elem);

    return result;
}

#endif
