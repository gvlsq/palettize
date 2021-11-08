#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#pragma warning(push, 0)
#include "stb_image.h"
#pragma warning(pop)

#include "palettize.h"

static palettize_config
ParseCommandLine(int ArgCount, char **Args)
{
    palettize_config Config = {};

    timestamp NowUTC = GetTimestampUTC();
    u32 Seed = (NowUTC.Year +
                NowUTC.Month*NowUTC.Day +
                NowUTC.Hour*(u32)Square((f32)NowUTC.Minute) +
                (u32)Cube((f32)NowUTC.Second - 0.025f)*0xFACADE);

    Config.SourcePath = 0;
    Config.ClusterCount = 5;
    Config.Seed = Seed;
    Config.SortType = SortType_Weight;
    Config.DestPath = "palette.bmp";

    if(ArgCount > 1)
    {
        Config.SourcePath = Args[1];
    }
    if(ArgCount > 2)
    {
        int ClusterCount = atoi(Args[2]);
        Config.ClusterCount = Clampi(1, ClusterCount, 64);
    }
    if(ArgCount > 3)
    {
        Config.Seed = (u32)atoi(Args[3]);
    }
    if(ArgCount > 4)
    {
        char *SortTypeString = Args[4];
        if(StringsMatch(SortTypeString, "red", false))
        {
            Config.SortType = SortType_Red;
        }
        else if(StringsMatch(SortTypeString, "green", false))
        {
            Config.SortType = SortType_Green;
        }
        else if(StringsMatch(SortTypeString, "blue", false))
        {
            Config.SortType = SortType_Blue;
        }
    }
    if(ArgCount > 5)
    {
        Config.DestPath = Args[5];
    }

    return(Config);
}

static bitmap
LoadAndScaleBitmap(char *Path, f32 MaxResizedDim)
{
    bitmap Result = {};

    int SourceWidth;
    int SourceHeight;
    void *SourceMemory = stbi_load(Path, &SourceWidth, &SourceHeight, 0, sizeof(u32));
    if(SourceMemory)
    {
        int SourcePitch = SourceWidth*sizeof(u32);

        f32 ScaleFactor = MaxResizedDim / (f32)Maximum(SourceWidth, SourceHeight);
        int ScaledWidth = RoundToInt(SourceWidth*ScaleFactor);
        int ScaledHeight = RoundToInt(SourceHeight*ScaleFactor);

        Result.Width = ScaledWidth;
        Result.Height = ScaledHeight;
        Result.Pitch = ScaledWidth*sizeof(u32);
        Result.Memory = malloc(Result.Pitch*ScaledHeight);

        u8 *Row = (u8 *)Result.Memory;
        for(int Y = 0;
            Y < ScaledHeight;
            Y++)
        {
            u32 *Texel = (u32 *)Row;
            for(int X = 0;
                X < ScaledWidth;
                X++)
            {
                f32 U = (f32)X / ((f32)ScaledWidth - 1.0f);
                f32 V = (f32)Y / ((f32)ScaledHeight - 1.0f);

                Assert((0.0f <= U) && (U <= 1.0f));
                Assert((0.0f <= V) && (V <= 1.0f));

                int SampleX = RoundToInt(U*((f32)SourceWidth - 1.0f));
                int SampleY = RoundToInt(V*((f32)SourceHeight - 1.0f));

                Assert((0 <= SampleX) && (SampleX < SourceWidth));
                Assert((0 <= SampleY) && (SampleY < SourceHeight));
                
                u32 Sample = *(u32 *)((u8 *)SourceMemory +
                                      (SampleX*sizeof(u32)) +
                                      (SampleY*SourcePitch));
                
                *Texel++ = Sample;
            }

            Row += Result.Pitch;
        }

        stbi_image_free(SourceMemory);
    }
    else
    {
        fprintf(stderr, "stb_image failed: %s\n", stbi_failure_reason());
    }

    return(Result);
}

static bitmap
AllocateBitmap(int Width, int Height)
{
    bitmap Result;
    Result.Memory = malloc(sizeof(u32)*Width*Height);
    Result.Width = Width;
    Result.Height = Height;
    Result.Pitch = sizeof(u32)*Width;
    
    return(Result);
}

static void
ClearObservations(cluster *Cluster)
{
    Cluster->ObservationSum = V3i(0, 0, 0);
    Cluster->ObservationCount = 0;
}

static void
InitializeKMeansContext(kmeans_context *Context, int ClusterCount)
{
    Context->ClusterCount = ClusterCount;
    Context->Clusters = (cluster *)malloc(sizeof(cluster)*ClusterCount);
}

static u32
AssignObservation(kmeans_context *Context, v3 Observation)
{
    f32 ClosestDistSquared = F32Max;
    cluster *ClosestCluster = 0;

    for(int ClusterIndex = 0;
        ClusterIndex < Context->ClusterCount;
        ClusterIndex++)
    {
        cluster *Cluster = Context->Clusters + ClusterIndex;

        f32 d = LengthSquared(Cluster->Centroid - Observation);
        if(d < ClosestDistSquared)
        {
            ClosestDistSquared = d;
            ClosestCluster = Cluster;
        }
    }

    Assert(ClosestCluster);

    ClosestCluster->ObservationSum += Observation;
    ClosestCluster->ObservationCount++;
    
    // Returning the index of the closest cluster for our early out in the loop
    // where this function is called
    u32 Result = (u32)(ClosestCluster - Context->Clusters);
    Assert(Result < (u32)Context->ClusterCount);
    
    return(Result);
}

static void
RecalculateCentroids(kmeans_context *Context)
{
    for(int ClusterIndex = 0;
        ClusterIndex < Context->ClusterCount;
        ClusterIndex++)
    {
        cluster *Cluster = Context->Clusters + ClusterIndex;

        // Assert(Cluster->ObservationCount);
        if(Cluster->ObservationCount)
        {
            Cluster->Centroid = Cluster->ObservationSum*(1.0f / Cluster->ObservationCount);
        }
        ClearObservations(Cluster);
    }
}

static void
SortClustersByCentroid(kmeans_context *Context, sort_type SortType)
{
    v3 FocalColor = V3i(0, 0, 0);
    switch(SortType)
    {
        case SortType_Red:
        {
            FocalColor = {53.23288178584245f,
                          80.10930952982204f,
                          67.22006831026425f};
        } break;
        
        case SortType_Green:
        {
            FocalColor = {87.73703347354422f,
                          -86.18463649762525f,
                          83.18116474777854f};
        } break;
        
        case SortType_Blue:
        {
            FocalColor = {32.302586667249486f,
                          79.19666178930935f,
                          -107.86368104495168f};
        } break;
    }

    for(int Outer = 0;
        Outer < Context->ClusterCount;
        Outer++)
    {
        b32 Swapped = false;
        
        for(int Inner = 0;
            Inner < (Context->ClusterCount - 1);
            Inner++)
        {
            cluster *ClusterA = Context->Clusters + Inner;
            cluster *ClusterB = Context->Clusters + Inner + 1;
            
            if(SortType == SortType_Weight)
            {
                if(ClusterB->ObservationCount > ClusterA->ObservationCount)
                {
                    cluster Swap = *ClusterA;
                    *ClusterA = *ClusterB;
                    *ClusterB = Swap;
                    
                    Swapped = true;
                }
            }
            else if((SortType == SortType_Red) ||
                    (SortType == SortType_Green) ||
                    (SortType == SortType_Blue))
            {
                f32 DistSquaredToColorA = LengthSquared(FocalColor - ClusterA->Centroid);
                f32 DistSquaredToColorB = LengthSquared(FocalColor - ClusterB->Centroid);
                if(DistSquaredToColorB < DistSquaredToColorA)
                {
                    cluster Swap = *ClusterA;
                    *ClusterA = *ClusterB;
                    *ClusterB = Swap;
                    
                    Swapped = true;
                }
            }
        }
        
        if(!Swapped)
        {
            break;
        }
    }
}

static int
ComputeTotalObservationCount(kmeans_context *Context)
{
    int Result = 0;

    for(int ClusterIndex = 0;
        ClusterIndex < Context->ClusterCount;
        ClusterIndex++)
    {
        cluster *Cluster = Context->Clusters + ClusterIndex;
        Result += Cluster->ObservationCount;
    }

    return(Result);
}

static void
ExportBMP(bitmap Bitmap, char *Path)
{
    FILE *File = fopen(Path, "wb");
    if(File)
    {
        // Swizzling the red and blue components of each texel because
        // that's how BMPs expect channels to be ordered
        u8 *Row = (u8 *)Bitmap.Memory;
        for(int Y = 0;
            Y < Bitmap.Height;
            Y++)
        {
            u32 *TexelPtr = (u32 *)Row;
            for(int X = 0;
                X < Bitmap.Width;
                X++)
            {
                u32 Texel = *TexelPtr;
                u32 Swizzled = ((((Texel >> 0) & 0xFF) << 16) |
                                (((Texel >> 8) & 0xFF) << 8) |
                                (((Texel >> 16) & 0xFF) << 0) |
                                (((Texel >> 24) & 0xFF) << 24));
                *TexelPtr++ = Swizzled;
            }
            
            Row += Bitmap.Pitch;
        }
        
        u32 BitmapSize = (u32)(Bitmap.Pitch*Bitmap.Height);
        
        bitmap_header BitmapHeader = {};
        BitmapHeader.Type = 0x4D42;
        BitmapHeader.FileSize = sizeof(BitmapHeader) + BitmapSize;
        BitmapHeader.OffBits = sizeof(BitmapHeader);
        BitmapHeader.Size = 40; // sizeof(BITMAPINFOHEADER)
        BitmapHeader.Width = Bitmap.Width;
        BitmapHeader.Height = Bitmap.Height;
        BitmapHeader.Planes = 1;
        BitmapHeader.BitCount = 32;
        BitmapHeader.Compression = BI_RGB;
        
        fwrite(&BitmapHeader, sizeof(BitmapHeader), 1,  File);
        fwrite(Bitmap.Memory, BitmapSize, 1, File);
        
        fclose(File);
    }
}

int
main(int ArgCount, char **Args)
{
    if(ArgCount > 1)
    {
        palettize_config Config = ParseCommandLine(ArgCount, Args);

        kmeans_context Context_;
        kmeans_context *Context = &Context_;
        InitializeKMeansContext(Context, Config.ClusterCount);

        // To improve performance, the source image is scaled such that its
        // largest dimension has a value of 100 pixels
        // @Refactor: Decouple scaling from loading so that small images are
        // not resized to be bigger
        bitmap Bitmap = LoadAndScaleBitmap(Config.SourcePath, 100.0f);
        
        bitmap PrevClusterIndexBuffer = AllocateBitmap(Bitmap.Width, Bitmap.Height);

        int PaletteWidth = 512;
        int PaletteHeight = 64;
        bitmap Palette = AllocateBitmap(PaletteWidth, PaletteHeight);

        if(Context->Clusters &&
           Bitmap.Memory &&
           PrevClusterIndexBuffer.Memory &&
           Palette.Memory)
        {
            random_series Entropy = SeedSeries(Config.Seed);
            for(int ClusterIndex = 0;
                ClusterIndex < Context->ClusterCount;
                ClusterIndex++)
            {
                cluster *Cluster = Context->Clusters + ClusterIndex;
                
                ClearObservations(Cluster);

                u32 SampleX = RandomU32Between(&Entropy, 0, (u32)(Bitmap.Width - 1));
                u32 SampleY = RandomU32Between(&Entropy, 0, (u32)(Bitmap.Height - 1));
                u32 Sample = *(u32 *)GetBitmapPtr(Bitmap, SampleX, SampleY);
    
                Cluster->Centroid = UnpackRGBAToCIELAB(Sample);
            }                

            int MinX = 0;
            int MinY = 0;
            int MaxX = Bitmap.Width;
            int MaxY = Bitmap.Height;

            b32 IteratedOnce = false;
            for(int Iteration = 0;
                ;
                Iteration++)
            {   
                b32 Changed = false;

                u8 *Row = (u8 *)GetBitmapPtr(Bitmap, MinX, MinY);
                u8 *PrevClusterIndexRow = (u8 *)GetBitmapPtr(PrevClusterIndexBuffer, MinX, MinY);
                for(int Y = MinY;
                    Y < MaxY;
                    Y++)
                {
                    u32 *TexelPtr = (u32 *)Row;
                    u32 *PrevClusterIndexPtr = (u32 *)PrevClusterIndexRow;
                    for(int X = MinX;
                        X < MaxX;
                        X++)
                    {
                        v3 TexelV3 = UnpackRGBAToCIELAB(*TexelPtr);
                        u32 ClusterIndex = AssignObservation(Context, TexelV3);

                        if(Iteration > 0)
                        {
                            u32 PrevClusterIndex = *PrevClusterIndexPtr;
                            if(ClusterIndex != PrevClusterIndex)
                            {
                                Changed = true;
                            }
                        }
                        *PrevClusterIndexPtr = ClusterIndex;

                        TexelPtr++;
                        PrevClusterIndexPtr++;
                    }

                    Row += Bitmap.Pitch;
                    PrevClusterIndexRow += PrevClusterIndexBuffer.Pitch;
                }

                if(Iteration == 0)
                {   
                    RecalculateCentroids(Context);
                }             
                else
                {    
                    if(Changed)
                    {
                        RecalculateCentroids(Context);
                    }
                    else
                    {
                        break;
                    }
                }
            }
      
            SortClustersByCentroid(Context, Config.SortType);
            
            u8 *ScanLine = (u8 *)malloc(sizeof(u32)*PaletteWidth);
            
            int TotalObservationCount = ComputeTotalObservationCount(Context);
            u32 *Row = (u32 *)ScanLine;
            for(int ClusterIndex = 0;
                ClusterIndex < Context->ClusterCount;
                ClusterIndex++)
            {
                cluster *Cluster = Context->Clusters + ClusterIndex;

                f32 Weight = SafeRatio0((f32)Cluster->ObservationCount,
                                        (f32)TotalObservationCount);
                int ClusterPixelWidth = RoundToInt(Weight*PaletteWidth);

                u32 CentroidColor = PackCIELABToRGBA(Cluster->Centroid);
                while(ClusterPixelWidth--)
                {
                    *Row++ = CentroidColor;
                }
            }
            
            u8 *DestRow = (u8 *)Palette.Memory;
            for(int Y = 0;
                Y < PaletteHeight;
                Y++)
            {
                u32 *SourceTexelPtr = (u32 *)ScanLine;
                u32 *DestTexelPtr = (u32 *)DestRow;
                for(int X = 0;
                    X < PaletteWidth;
                    X++)
                {
                    *DestTexelPtr++ = *SourceTexelPtr++;
                }
                
                DestRow += Palette.Pitch;
            }
            
            ExportBMP(Palette, Config.DestPath);
        }
        else
        {
            fprintf(stderr, "Error: malloc failed at startup\n");
        }
    }
    else
    {
        fprintf(stderr, "Usage: %s [source path] [cluster count] [seed] [sort type] [dest path]\n", Args[0]);
    }
    
    return(0);
}
