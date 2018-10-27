#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <iostream>
#include <string>
#include <vector>
#include "vec.hpp"

// デバッグ用print関数
template <class... A>
void print() {
    std::cout << std::endl;
}
template <class... A>
void prints_rest() {
    std::cout << std::endl;
}
template <class T, class... A>
void prints_rest(const T &first, const A &... rest) {
    std::cout << " " << first;
    prints_rest(rest...);
}
template <class T, class... A>
void print(const T &first, const A &... rest) {
    std::cout << first;
    prints_rest(rest...);
}

struct Image {
    int width_res, height_res;
    std::vector<Color> color_vec;

    Image();
    Image(const int width_res, const int height_res);

    inline void set_image_size(const int width_res, const int height_res) {
        this->width_res = width_res;
        this->height_res = height_res;
        this->color_vec.resize(width_res * height_res);
    }

    void flip();

    inline Color get_color(const int row, const int column) const {
        return color_vec[row * width_res + column];
    }
    inline void set_color(const Color &color, const int row, const int column) {
        color_vec[row * width_res + column] = color;
    }
};

std::vector<std::string> split(const std::string &s, const char delim);

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

std::vector<double> linspace(const double start, const double end, const int n,
                             const bool endpoint = true);

Image load_ascii_ppm_file(const std::string &filename);
Image load_ppm_P6_file(const std::string &filename);
void save_ppm_file(const std::string &filename, const Image &image);

#endif
