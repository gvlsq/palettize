#ifndef PALETTIZE_RANDOM_H
#define PALETTIZE_RANDOM_H

struct Random_Series {
    u32 seed;
};

inline Random_Series seed_series(u32 seed) {
    Random_Series result;
    result.seed = seed;

    return result;
}

inline u32 random_u32(Random_Series *series) {
    // 32-bit Xorshift
    u32 result = series->seed;
    result ^= result << 13;
    result ^= result >> 17;
    result ^= result << 5;

    series->seed = result;

    return result;
}

inline u32 random_u32_between(Random_Series *series, u32 min, u32 max) {
    u32 result = min + (random_u32(series) % (max - min));
    assert(min <= result && result <= max);

    return result;
}

#endif
