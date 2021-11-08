#if !defined(PALETTIZE_TIME_H)

#include <time.h>

struct timestamp
{
    int Year;
    int Month;
    int Day;
    int Hour;
    int Minute;
    int Second;
};

inline timestamp
GetTimestampUTC(void)
{
    time_t Time = time(0);
    tm *TM = gmtime(&Time);

    timestamp Result;
    Result.Year = TM->tm_year;
    Result.Month = TM->tm_mon;
    Result.Day = TM->tm_mday;
    Result.Hour = TM->tm_hour;
    Result.Minute = TM->tm_min;
    Result.Second = TM->tm_sec;

    return(Result);
}

#define PALETTIZE_TIME_H
#endif
