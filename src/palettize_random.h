#if !defined(PALETTIZE_RANDOM_H)

struct random_series
{
    u32 Seed;
};

inline random_series
SeedSeries(u32 Seed)
{
    random_series Result;
    Result.Seed = Seed;
    
    return(Result);
}

inline u32
RandomU32(random_series *Series)
{
    // 32-bit Xorshift
    u32 Result = Series->Seed;
	Result ^= Result << 13;
	Result ^= Result >> 17;
    Result ^= Result << 5;
    
    Series->Seed = Result;
    
    return(Result);
}

inline u32
RandomU32Between(random_series *Series, u32 Min, u32 Max)
{
    u32 Result = (Min + (RandomU32(Series) % (Max - Min)));
    Assert((Min <= Result) && (Result <= Max));

    return(Result);
}

#define PALETTIZE_RANDOM_H
#endif
