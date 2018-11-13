#ifndef _LATTICENOISE_H_
#define _LATTICENOISE_H_

#include <vector>
#include "random.hpp"
#include "utility.hpp"
#include "vec.hpp"

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

    LatticeNoise();
    virtual ~LatticeNoise();

    void set_num_octaves(const int &num_octaves);
    void set_fs_range(const double &fs_min, const double &fs_max);
    void set_fBm_range(const double &fBm_min, const double &fBm_max);
    void set_gain(const double &gain);
    void set_lacunarity(const double &lacunarity);

    virtual double value_noise(const Vec &p) const = 0;

    inline unsigned char perm(const int idx) const { return permutation_table[idx & TABLE_MASK]; }
    inline int index(const int ix, const int iy, const int iz) const {
        return perm(ix + perm(iy + perm(iz)));
    }

    void init_value_table(int seed);
    double value_fractal_sum(const Vec &p) const;
    double value_turbulence(const Vec &p) const;
    double value_fBm(const Vec &p) const;
};

struct LinearNoise : public LatticeNoise {
    LinearNoise();

    inline double lerp(const double x, const double a, const double b) const {
        return a + (b - a) * x;
    }

    double value_noise(const Vec &p) const;
};

struct CubicNoise : public LatticeNoise {
    CubicNoise();

    inline double four_knot_spline(const double x, const double knots[]) const {
        const double c3 = -0.5 * knots[0] + 1.5 * knots[1] - 1.5 * knots[2] + 0.5 * knots[3];
        const double c2 = knots[0] - 2.5 * knots[1] + 2.0 * knots[2] - 0.5 * knots[3];
        const double c1 = 0.5 * (-knots[0] + knots[2]);
        const double c0 = knots[1];

        return ((c3 * x + c2) * x + c1) * x + c0;
    }

    double value_noise(const Vec &p) const;
};

#endif
