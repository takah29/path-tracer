#include <array>
#include <fstream>
#include <string>
#include <tuple>
#include "utility.h"

using RGBE = unsigned char[4];

inline std::tuple<double, double, double> convert_components(RGBE rgbe) {
    double e = pow(2, rgbe[3] - 128);
    double red = e * rgbe[0] / 256.0;
    double green = e * rgbe[1] / 256.0;
    double blue = e * rgbe[2] / 256.0;
    return std::make_tuple(red, green, blue);
}

bool decode(std::vector<RGBE> &scanline, int len, std::ifstream &infile);
void getline_for_binary(std::ifstream &infile, std::array<char, 200> &line);
bool load_hdr_image(const std::string filename, Image &image);
