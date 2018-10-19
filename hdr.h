#include <array>
#include <fstream>
#include <string>
#include <tuple>
#include "utility.h"

using RGBE = unsigned char[4];

inline std::tuple<double, double, double> convert_components(RGBE rgbe) {
    double e = static_cast<double>(pow(2, rgbe[3] - 128));
    double red = e * rgbe[0] / 256.0;
    double green = e * rgbe[1] / 256.0;
    double blue = e * rgbe[2] / 256.0;
    return std::make_tuple(red, green, blue);
}

bool decrunch(std::vector<RGBE> &scanline, int len, std::ifstream &infile) {
    char tmp_c;
    infile.get(tmp_c);
    infile.get(tmp_c);
    infile.get(tmp_c);
    infile.get(tmp_c);

    unsigned char code, val;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < len;) {
            infile.get(tmp_c);
            code = tmp_c;
            if (code > 128) {  // run
                code -= 128;
                infile.get(tmp_c);
                val = tmp_c;
                while (code--) scanline[j++][i] = val;
            } else {  // non-run
                while (code--) {
                    infile.get(tmp_c);
                    scanline[j++][i] = static_cast<unsigned char>(tmp_c);
                }
            }
        }
    }

    return infile.eof() ? false : true;
}

// バイナリファイルを改行コード区切りで読み込む
void getline_for_binary(std::ifstream &infile, std::array<char, 200> &line) {
    line.fill(0);
    int i = 0;
    char c;
    while (true) {
        infile.get(c);
        line[i++] = c;
        if (c == 0x0a) break;
    }
}

bool load_hdr_image(const std::string filename, Image &image) {
    // バイナリモードで開く
    std::ifstream infile(filename, std::ios::in | std::ios::binary);
    if (!infile) return false;

    std::array<char, 200> line;
    getline_for_binary(infile, line);

    // ファイルフォーマットチェック
    if (std::string(line.data(), 10) != "#?RADIANCE") {
        infile.close();
        return false;
    }

    while (true) {
        getline_for_binary(infile, line);
        if (line[0] == 0x0a) break;
    }

    // 画像サイズ取得
    getline_for_binary(infile, line);
    int width_res, height_res;
    if (!sscanf(line.data(), "-Y %d +X %d", &height_res, &width_res)) {
        infile.close();
        return false;
    }

    image.set_image_size(width_res, height_res);
    std::vector<RGBE> scanline(width_res);
    double red, green, blue;
    for (int i = 0; i < height_res; i++) {
        if (decrunch(scanline, width_res, infile) == false) break;
        for (int j = 0; j < width_res; j++) {
            std::tie(red, green, blue) = convert_components(scanline[j]);
            image.set_color(Color(red, green, blue), i, j);
        }
    }

    infile.close();
    return true;
}
