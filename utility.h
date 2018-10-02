#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "vec.h"

inline double clamp(double x) {
    if (x > 1.0) {
        return 1.0;
    } else if (x < 0.0) {
        return 0.0;
    } else {
        return x;
    }
}

inline int to_int(double x) { return static_cast<int>(pow(clamp(x), 1 / 2.2) * 255.0 + 0.5); }

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
