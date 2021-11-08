#if !defined(PALETTIZE_STRING_H)

inline char
FlipCase(char C)
{
    char Result = '\0';

    if(('a' <= C) && (C <= 'z'))
    {
        Result = (C - 'a') + 'A';
    }
    else if(('A' <= C) && (C <= 'Z'))
    {
        Result = (C - 'A') + 'a';
    }
    
    return(Result);
}

inline b32
StringsMatch(char *A, char *B, b32 CaseSensitive = true)
{
    while(*A && *B)
    {
        if((*A == *B) ||
           ((CaseSensitive == false) &&
            (*A == FlipCase(*B))))
        {
            A++;
            B++;
        }
        else
        {
            break;
        }
    }
    
    b32 Result = *A == *B;

    return(Result);
}

#define PALETTIZE_STRING_H
#endif
