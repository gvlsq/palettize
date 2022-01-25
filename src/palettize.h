#ifndef PALETTIZE_H
#define PALETTIZE_H

#include <assert.h>
#include <stdint.h>

#define Invalid_Code_Path assert(!"Invalid code path")
#define Invalid_Default_Case default: {Invalid_Code_Path;} break

#define array_count(a) (sizeof((a)) / sizeof((a)[0]))

#define maximum(a, b) ((a) > (b) ? (a) : (b))
#define minimum(a, b) ((a) < (b) ? (a) : (b))

typedef int32_t s32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#include "palettize_math.h"
#include "palettize_random.h"
#include "palettize_string.h"

enum Sort_Type {
    SORT_TYPE_WEIGHT,
    SORT_TYPE_RED,
    SORT_TYPE_GREEN,
    SORT_TYPE_BLUE,
};

struct Palettize_Config {
    char *source_path;
    int cluster_count;
    u32 seed;
    Sort_Type sort_type;
    char *dest_path;
};

#define get_bitmap_ptr(b, x, y) ((u8 *)(b).memory + (sizeof(u32)*(x)) + ((y)*(b).pitch))
struct Bitmap {
    void *memory;
    int width;
    int height;
    int pitch;
};

struct KMeans_Cluster {
    Vector3 centroid;

    int observation_count;
    int observation_capacity;
    Vector3 *observations;
};

#pragma pack(push, 1)
#define BI_RGB 0x0000
struct Bitmap_Header {
    u16 type;
    u32 file_size;
    u16 reserved1;
    u16 reserved2;
    u32 off_bits;

    u32 size;
    s32 width;
    s32 height;
    u16 planes;
    u16 bit_count;
    u32 compression;
    u32 size_image;
    s32 x_pels_per_meter;
    s32 y_pels_per_meter;
    u32 clr_used;
    u32 clr_important;
};
#pragma pack(pop)

#endif
