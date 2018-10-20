#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "vec.h"

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

    Image() : width_res(-1), height_res(-1), color_vec(0) {}
    Image(const int &width_res, const int &height_res)
        : width_res(width_res), height_res(height_res), color_vec(height_res * width_res) {}

    inline void set_image_size(const int &width_res, const int &height_res) {
        this->width_res = width_res;
        this->height_res = height_res;
        this->color_vec.resize(width_res * height_res);
    }

    void flip() {
        std::vector<Color> tmp_color_vec(width_res * height_res);
        for (int i = 0; i < height_res; i++) {
            for (int j = 0; j < width_res; j++) {
                tmp_color_vec[i * width_res + j] = color_vec[i * width_res + (width_res - 1 - j)];
            }
        }
        color_vec = tmp_color_vec;
    }

    inline Color get_color(const int &row, const int &column) const {
        return color_vec[row * width_res + column];
    }
    inline void set_color(const Color &color, const int &row, const int &column) {
        color_vec[row * width_res + column] = color;
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

std::vector<double> linspace(double start, double end, int n, bool endpoint=true) {
    double delta = endpoint == true ? (end - start) / (n - 1) : (end - start) / n;
    std::vector<double> result(n);
    for (int i = 0; i < n; i++) {
        result[i] = start + i * delta;
    }
    return result;
}

Image load_ascii_ppm_file(const std::string &filename) {
    std::ifstream infile(filename);
    std::string line;

    getline(infile, line);
    if (line != "P3") {
        printf("Error: Expected file type is P3.\n");
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

Image load_ppm_P6_file(const std::string &filename) {
    std::ifstream infile(filename, std::ios_base::in | std::ios_base::binary);
    std::string line;

    getline(infile, line);
    // printf("%s", line.c_str());
    if (line != "P6") {
        printf("Error: Expected file type is P3.\n");
        exit(1);
    }

    int width_res, height_res;
    infile >> width_res >> height_res;
    printf("%d\n", width_res);
    int max_value;
    infile >> max_value;
    printf("%d\n", max_value);

    Image image(width_res, height_res);
    char r, g, b;
    for (int i = 0; i < height_res; i++) {
        for (int j = 0; j < width_res; j++) {
            infile.read((char *)&r, 1);
            infile.read((char *)&g, 1);
            infile.read((char *)&b, 1);
            image.set_color(
                Color((double)r / max_value, (double)g / max_value, (double)b / max_value), i, j);
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
