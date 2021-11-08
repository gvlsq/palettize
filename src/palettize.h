#if !defined(PALETTIZE_H)

#include <assert.h>
#include <stdint.h>

#if defined(PALETTIZE_DEBUG)
#define Assert assert
#else
#define Assert
#endif
#define InvalidCodePath Assert(!"InvalidCodePath")
#define InvalidDefaultCase default: {InvalidCodePath;} break

#define ArrayCount(Array) (sizeof((Array)) / sizeof((Array)[0]))

#define Maximum(A, B) ((A) > (B) ? (A) : (B))
#define Minimum(A, B) ((A) < (B) ? (A) : (B))

typedef int32_t s32;
typedef s32 b32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef uintptr_t umm;

typedef float f32;

#include "palettize_math.h"
#include "palettize_random.h"
#include "palettize_string.h"
#include "palettize_time.h"

enum sort_type
{
    SortType_Weight,
    SortType_Red,
    SortType_Green,
    SortType_Blue,
};

struct palettize_config
{
    char *SourcePath;
    int ClusterCount;
    u32 Seed;
    sort_type SortType;
    char *DestPath;
};

#define GetBitmapPtr(Bitmap, X, Y) ((u8 *)(Bitmap).Memory + (sizeof(u32)*(X)) + ((Y)*(Bitmap).Pitch))
struct bitmap
{
    void *Memory;

    int Width;
    int Height;
    
    int Pitch;
};

struct cluster
{
    v3 Centroid;
    
    v3 ObservationSum;
    int ObservationCount;
};
struct kmeans_context
{
    int ClusterCount;
    cluster *Clusters;
};

#pragma pack(push, 1)
#define BI_RGB 0x0000
struct bitmap_header
{
    u16 Type;
    u32 FileSize;
    u16 Reserved1;
    u16 Reserved2;
    u32 OffBits;
    
    u32 Size;
    s32 Width;
    s32 Height;
    u16 Planes;
    u16 BitCount;
    u32 Compression;
    u32 SizeImage;
    s32 XPelsPerMeter;
    s32 YPelsPerMeter;
    u32 ClrUsed;
    u32 ClrImportant;
};
#pragma pack(pop)

#define PALETTIZE_H
#endif
