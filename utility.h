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
    inline void set_color(const Color &color, const int &row, const int &column) {
        data[row * height_res + column] = color;
    }
};

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

Image load_ppm_file(const std::string &filename) {
    std::ifstream infile(filename);
    std::string line;

    getline(infile, line);
    if (line != "P3") {
        printf("Error: Expected file type is P3.");
        exit(1);
    }

    int width_res, height_res;
    infile >> width_res >> height_res;
    double max_value;
    infile >> max_value;

    Image image(width_res, height_res);
    int r, g, b;
    for (int i = 0; i < height_res; i++) {
        for (int j = 0; j < width_res; j++) {
            infile >> r >> g >> b;
            image.set_color(Color(r / max_value, g / max_value, b / max_value), i, j);
        }
    }

    return image;
}

void save_ppm_file(const std::string &filename, const Image &image) {
    std::ofstream outfile(filename, std::ios_base::out);

    outfile << "P3" << std::endl;
    outfile << image.width_res << " " << image.height_res << std::endl;
    outfile << "255" << std::endl;

    for (int i = 0; i < image.height_res; i++) {
        for (int j = 0; j < image.width_res; j++) {
            Color c(image.get_color(i, j));
            outfile << to_int(c.x) << " " << to_int(c.y) << " " << to_int(c.z) << std::endl;
        }
    }

    outfile.close();
}

#endif
