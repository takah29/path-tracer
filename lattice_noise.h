#ifndef _LATTICENOISE_H_
#define _LATTICENOISE_H_

#include <vector>
#include "random.h"
#include "utility.h"
#include "vec.h"

const int TABLE_SIZE = 256;
const int TABLE_MASK = TABLE_SIZE - 1;
const int SEED_VALUE = 253;

struct LatticeNoise {
    const std::vector<unsigned char> permutation_table;
    std::vector<double> value_table;
    int num_octaves;
    double fs_min, fs_max;
    double gain, lacunarity;
    double fBm_min, fBm_max;

    LatticeNoise()
        : permutation_table(
              {225, 155, 210, 108, 175, 199, 221, 144, 203, 116, 70,  213, 69,  158, 33,  252,
               5,   82,  173, 133, 222, 139, 174, 27,  9,   71,  90,  246, 75,  130, 91,  191,
               169, 138, 2,   151, 194, 235, 81,  7,   25,  113, 228, 159, 205, 253, 134, 142,
               248, 65,  224, 217, 22,  121, 229, 63,  89,  103, 96,  104, 156, 17,  201, 129,
               36,  8,   165, 110, 237, 117, 231, 56,  132, 211, 152, 20,  181, 111, 239, 218,
               170, 163, 51,  172, 157, 47,  80,  212, 176, 250, 87,  49,  99,  242, 136, 189,
               162, 115, 44,  43,  124, 94,  150, 16,  141, 247, 32,  10,  198, 223, 255, 72,
               53,  131, 84,  57,  220, 197, 58,  50,  208, 11,  241, 28,  3,   192, 62,  202,
               18,  215, 153, 24,  76,  41,  15,  179, 39,  46,  55,  6,   128, 167, 23,  188,
               106, 34,  187, 140, 164, 73,  112, 182, 244, 195, 227, 13,  35,  77,  196, 185,
               26,  200, 226, 119, 31,  123, 168, 125, 249, 68,  183, 230, 177, 135, 160, 180,
               12,  1,   243, 148, 102, 166, 38,  238, 251, 37,  240, 126, 64,  74,  161, 40,
               184, 149, 171, 178, 101, 66,  29,  59,  146, 61,  254, 107, 42,  86,  154, 4,
               236, 232, 120, 21,  233, 209, 45,  98,  193, 114, 78,  19,  206, 14,  118, 127,
               48,  79,  147, 85,  30,  207, 219, 54,  88,  234, 190, 122, 95,  67,  143, 109,
               137, 214, 145, 93,  92,  100, 245, 0,   216, 186, 60,  83,  105, 97,  204, 52}),
          value_table(TABLE_SIZE),
          num_octaves(8),
          fs_min(-1.0),
          fs_max(1.0),
          gain(0.5),
          lacunarity(3.0),
          fBm_min(-1.0),
          fBm_max(1.0) {
        init_value_table(SEED_VALUE);
    }
    virtual ~LatticeNoise() {}

    void set_num_octaves(const int &num_octaves) { this->num_octaves = num_octaves; }
    void set_fs_range(const double &fs_min, const double &fs_max) {
        this->fs_min = fs_min;
        this->fs_max = fs_max;
    }
    void set_fBm_range(const double &fBm_min, const double &fBm_max) {
        this->fBm_min = fBm_min;
        this->fBm_max = fBm_max;
    }
    void set_gain(const double &gain) { this->gain = gain; }
    void set_lacunarity(const double &lacunarity) { this->lacunarity = lacunarity; }

    virtual double value_noise(const Vec &p) const = 0;

    inline unsigned char perm(const int idx) const { return permutation_table[idx & TABLE_MASK]; }
    inline int index(const int ix, const int iy, const int iz) const {
        return perm(ix + perm(iy + perm(iz)));
    }

    void init_value_table(int seed) {
        UniformRealGenerator rnd(seed, -1.0, 1.0);
        for (int i = 0; i < TABLE_SIZE; i++) {
            value_table[i] = rnd();
        }
    }

    double value_fractal_sum(const Vec &p) const {
        double amplitude = 1.0;
        double frequency = 1.0;
        double fractal_sum = 0.0;

        for (int i = 0; i < num_octaves; i++) {
            fractal_sum += amplitude * value_noise(frequency * p);
            amplitude *= 0.5;
            frequency *= 2.0;
        }

        fractal_sum = (fractal_sum - fs_min) / (fs_max - fs_min);
        return fractal_sum;
    }

    double value_turbulence(const Vec &p) const {
        double amplitude = 1.0;
        double frequency = 1.0;
        double turbulence = 0.0;

        for (int i = 0; i < num_octaves; i++) {
            turbulence += amplitude * std::fabs(value_noise(frequency * p));
            amplitude *= 0.5;
            frequency *= 2.0;
        }

        turbulence = (turbulence - fs_min) / (fs_max - fs_min);
        return turbulence;
    }

    double value_fBm(const Vec &p) const {
        double amplitude = 1.0;
        double frequency = 1.0;
        double fBm = 0.0;

        for (int i = 0; i < num_octaves; i++) {
            fBm += amplitude * value_noise(frequency * p);
            amplitude *= gain;
            frequency *= lacunarity;
        }

        fBm = (fBm - fBm_min) / (fBm_max - fBm_min);
        return fBm;
    }
};

struct LinearNoise : public LatticeNoise {
    LinearNoise() : LatticeNoise() {}

    inline double lerp(const double x, const double a, const double b) const {
        return a + (b - a) * x;
    }

    double value_noise(const Vec &p) const {
        int ix, iy, iz;
        ix = floor(p.x);
        iy = floor(p.y);
        iz = floor(p.z);

        double fx, fy, fz;
        fx = p.x - ix;
        fy = p.y - iy;
        fz = p.z - iz;

        double d[2][2][2];
        for (int k = 0; k <= 1; k++) {
            for (int j = 0; j <= 1; j++) {
                for (int i = 0; i <= 1; i++) {
                    d[k][j][i] = value_table[index(ix + i, iy + j, iz + k)];
                }
            }
        }

        double x0, x1, x2, x3, y0, y1, z0;
        x0 = lerp(fx, d[0][0][0], d[0][0][1]);
        x1 = lerp(fx, d[0][1][0], d[0][1][1]);
        x2 = lerp(fx, d[1][0][0], d[1][0][1]);
        x3 = lerp(fx, d[1][1][0], d[1][1][1]);
        y0 = lerp(fy, x0, x1);
        y1 = lerp(fy, x2, x3);
        z0 = lerp(fz, y0, y1);

        return z0;
    }
};

struct CubicNoise : public LatticeNoise {
    CubicNoise() : LatticeNoise() {}

    inline double four_knot_spline(const double x, const double knots[]) const {
        const double c3 = -0.5 * knots[0] + 1.5 * knots[1] - 1.5 * knots[2] + 0.5 * knots[3];
        const double c2 = knots[0] - 2.5 * knots[1] + 2.0 * knots[2] - 0.5 * knots[3];
        const double c1 = 0.5 * (-knots[0] + knots[2]);
        const double c0 = knots[1];

        return ((c3 * x + c2) * x + c1) * x + c0;
    }

    double value_noise(const Vec &p) const {
        int ix, iy, iz;
        ix = floor(p.x);
        iy = floor(p.y);
        iz = floor(p.z);

        double fx, fy, fz;
        fx = p.x - ix;
        fy = p.y - iy;
        fz = p.z - iz;

        double xknots[4], yknots[4], zknots[4];
        for (int k = -1; k <= 2; k++) {
            for (int j = -1; j <= 2; j++) {
                for (int i = -1; i <= 2; i++) {
                    xknots[i + 1] = value_table[index(ix + i, iy + j, iz + k)];
                }
                yknots[j + 1] = four_knot_spline(fx, xknots);
            }
            zknots[k + 1] = four_knot_spline(fy, yknots);
        }

        return clamp(four_knot_spline(fz, zknots), -1.0, 1.0);
    }
};

#endif
