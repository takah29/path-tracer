#include "utility.hpp"
#include <fstream>

Image::Image() : width_res(-1), height_res(-1), color_vec(0) {}
Image::Image(const int width_res, const int height_res)
    : width_res(width_res), height_res(height_res), color_vec(height_res * width_res) {}

void Image::flip() {
    std::vector<Color> tmp_color_vec(width_res * height_res);
    for (int i = 0; i < height_res; i++) {
        for (int j = 0; j < width_res; j++) {
            tmp_color_vec[i * width_res + j] = color_vec[i * width_res + (width_res - 1 - j)];
        }
    }
    color_vec = tmp_color_vec;
}

std::string strip(std::string &s, const std::string trim_str) {
    std::string result;

    std::string::size_type left = s.find_first_not_of(trim_str);
    if (left != std::string::npos) {
        std::string::size_type right = s.find_last_not_of(trim_str);
        result = s.substr(left, right - left + 1);
    }
    return result;
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

std::vector<std::string> split_reg(const std::string &s, const std::string regex_delim) {
    std::vector<std::string> v;

    std::regex separator{regex_delim};
    auto ite = std::sregex_token_iterator(s.begin(), s.end(), separator, -1);
    auto end = std::sregex_token_iterator();
    while (ite != end) {
        v.push_back(*ite++);  // 分割文字列を格納
    }
    return v;
}

std::vector<double> linspace(const double start, const double end, const int n,
                             const bool endpoint) {
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
        printf("Error: Expected file type is P6.\n");
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
                Color(double(r) / max_value, double(g) / max_value, double(b) / max_value), i, j);
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
