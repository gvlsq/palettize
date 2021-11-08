#if !defined(PALETTIZE_MATH_H)

#include <float.h>
#include <math.h>

#define F32Max FLT_MAX
#define F32Epsilon FLT_EPSILON

struct v3
{
    f32 x, y, z;
};

struct m3x3
{
    v3 XAxis;
    v3 YAxis;
    v3 ZAxis;
};

inline v3
V3(f32 X, f32 Y, f32 Z)
{
    v3 Result;
    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    
    return(Result);
}

inline v3
V3i(int X, int Y, int Z)
{
    v3 Result = V3((f32)X, (f32)Y, (f32)Z);
    
    return(Result);
}

// 
// Scalar operations
// 

inline f32
Abs(f32 S)
{
    f32 Result = fabsf(S);

    return(Result);
}

inline f32
Clamp(f32 Min, f32 S, f32 Max)
{
    f32 Result = S;

    if(Result < Min)
    {
        Result = Min;
    }
    else if(Result > Max)
    {
        Result = Max;
    }

    return(Result);
}

inline int
Clampi(int Min, int S, int Max)
{
    int Result = (int)Clamp((f32)Min, (f32)S, (f32)Max);
    
    return(Result);
}

inline f32
Clamp01(f32 S)
{
    f32 Result = Clamp(0.0f, S, 1.0f);
    
    return(Result);
}

inline f32
Cube(f32 S)
{
    f32 Result = S*S*S;

    return(Result);
}

inline f32
CubeRoot(f32 S)
{
    f32 Result = cbrtf(S);
    
    return(Result);
}

inline b32
EqualsApproximately(float A, float B)
{
    b32 Result = Abs(B - A) <= F32Epsilon;

    return(Result);
}

inline f32
Pow(f32 X, f32 Y)
{
    f32 Result = powf(X, Y);
    
    return(Result);
}

inline f32
Round(f32 S)
{
    f32 Result = roundf(S);

    return(Result);
}

inline int
RoundToInt(f32 S)
{
    int Result = (int)Round(S);
    
    return(Result);
}

inline u32
RoundToU32(f32 S)
{
    u32 Result = (u32)Round(S);
    
    return(Result);
}

inline f32
SafeRatioN(f32 Dividend, f32 Divisor, f32 N)
{
    f32 Result = N;
    if(Divisor != 0.0f)
    {
        Result = (Dividend / Divisor);
    }

    return(Result);
}

inline f32
SafeRatio0(f32 Dividend, f32 Divisor)
{
    f32 Result = SafeRatioN(Dividend, Divisor, 0.0f);
    
    return(Result);
}

inline f32
Square(f32 S)
{
    f32 Result = S*S;
    
    return(Result);
}

inline f32
SquareRoot(f32 S)
{
    f32 Result = sqrtf(S);
    
    return(Result);
}

// 
// v3 operations
// 

inline v3
operator+(v3 A, v3 B)
{
    v3 Result;
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    
    return(Result);
}

inline v3
operator-(v3 A, v3 B)
{
    v3 Result;
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    
    return(Result);
}

inline v3
operator*(v3 A, f32 S)
{
    v3 Result;
    Result.x = A.x*S;
    Result.y = A.y*S;
    Result.z = A.z*S;
    
    return(Result);
}

inline void
operator+=(v3 &A, v3 B)
{
    A = A + B;
}

inline void
operator*=(v3 &A, f32 S)
{
    A = A*S;
}

inline f32
Dot(v3 A, v3 B)
{
    f32 Result = ((A.x*B.x) +
                  (A.y*B.y) +
                  (A.z*B.z));
    return(Result);
}

inline b32
EqualsApproximately(v3 A, v3 B)
{
    b32 Result = (EqualsApproximately(A.x, B.x) &&
                  EqualsApproximately(A.y, B.y) &&
                  EqualsApproximately(A.z, B.z));
    return(Result);
}

inline f32
LengthSquared(v3 V)
{
    f32 Result = Dot(V, V);
    
    return(Result);
}

inline f32 LinearRGBTosRGB(f32 V);
inline v3
LinearRGBTosRGB(v3 V)
{
    v3 Result;
    Result.x = LinearRGBTosRGB(V.x);
    Result.y = LinearRGBTosRGB(V.y);
    Result.z = LinearRGBTosRGB(V.z);
    
    return(Result);
}

inline u32
PackRGBA(v3 V)
{
    u32 Result = ((RoundToU32(V.x*255.0f) << 0) |
                  (RoundToU32(V.y*255.0f) << 8) |
                  (RoundToU32(V.z*255.0f) << 16) |
                  (255 << 24));
    return(Result);
}

inline f32 sRGBToLinearRGB(f32 Channel);
inline v3
sRGBToLinearRGB(v3 V)
{
    v3 Result;
    Result.x = sRGBToLinearRGB(V.x);
    Result.y = sRGBToLinearRGB(V.y);
    Result.z = sRGBToLinearRGB(V.z);
    
    return(Result);
}

inline v3
UnpackRGBA(u32 U)
{
    f32 Inv255 = 1.0f / 255.0f;

    v3 Result;
    Result.x = (((U >> 0) & 0xFF)*Inv255);
    Result.y = (((U >> 8) & 0xFF)*Inv255);
    Result.z = (((U >> 16) & 0xFF)*Inv255);
    
    return(Result);
}

// 
// m3x3 operations
// 

inline v3
operator*(m3x3 M, v3 V)
{
    v3 Result;
    Result.x = Dot(V3(M.XAxis.x, M.YAxis.x, M.ZAxis.x), V);
    Result.y = Dot(V3(M.XAxis.y, M.YAxis.y, M.ZAxis.y), V);
    Result.z = Dot(V3(M.XAxis.z, M.YAxis.z, M.ZAxis.z), V);
    
    return(Result);
}

// 
// Color space operations
// 

// White point coords for Illuminant D65
#define Xn 0.950470f
#define Yn 1.0f
#define Zn 1.088830f

inline f32
InvFt(f32 t)
{
    f32 Result;
    f32 Sigma = (6.0f / 29.0f);
    if(t > Sigma)
    {
        Result = Cube(t);
    }
    else
    {
        Result = ((3.0f*Square(Sigma))*(t - (4.0f / 29.0f)));
    }
    
    return(Result);
}

inline v3
CIELABToCIEXYZ(v3 V)
{
    v3 Result;
    Result.x = (Xn*InvFt(((V.x + 16.0f) / 116.0f) + (V.y / 500.0f)));
    Result.y = (Yn*InvFt((V.x + 16.0f) / 116.0f));
    Result.z = (Zn*InvFt(((V.x + 16.0f) / 116.0f) - (V.z / 200.0f)));
    
    return(Result);
}

inline f32
Ft(f32 t)
{
    f32 Result;
    f32 Sigma = (6.0f / 29.0f);
    if(t > Cube(Sigma))
    {
        Result = CubeRoot(t);
    }
    else
    {
        Result = ((t / (3.0f*Square(Sigma))) + (4.0f / 29.0f));
    }
    
    return(Result);
}

inline v3
CIEXYZToCIELAB(v3 V)
{
    v3 Result;
    Result.x = (116.0f*Ft(V.y / Yn) - 16.0f);
    Result.y = (500.0f*(Ft(V.x / Xn) - Ft(V.y / Yn)));
    Result.z = (200.0f*(Ft(V.y / Yn) - Ft(V.z / Zn)));
    
    return(Result);
}

inline v3
CIEXYZToLinearRGB(v3 V)
{
    m3x3 Matrix;
    Matrix.XAxis = V3(3.2404542f, -0.9692660f, 0.0556434f);
    Matrix.YAxis = V3(-1.5371385f, 1.8760108f, -0.2040259f);
    Matrix.ZAxis = V3(-0.4985314f, 0.0415560f, 1.0572252f);
    v3 Result = Matrix*V;
    
    return(Result);
}

inline v3
LinearRGBToCIEXYZ(v3 V)
{
    m3x3 Matrix;
    Matrix.XAxis = V3(0.4124564f, 0.2126729f, 0.0193339f);
    Matrix.YAxis = V3(0.3575761f, 0.7151522f, 0.1191920f);
    Matrix.ZAxis = V3(0.1804375f, 0.0721750f, 0.9503041f);
    v3 Result = Matrix*V;
    
    return(Result);
}

inline f32
LinearRGBTosRGB(f32 S)
{
    S = Clamp01(S);
    
    f32 Result;
    if(S <= 0.0031308f)
    {
        Result = 12.92f*S;
    }
    else
    {
        Result = ((1.055f*Pow(S, (1.0f / 2.4f))) - 0.055f);
    }
    
    return(Result);
}

inline u32
PackCIELABToRGBA(v3 V)
{
    v3 CIELAB = V;
    v3 CIEXYZ = CIELABToCIEXYZ(CIELAB);
    v3 LinearRGB = CIEXYZToLinearRGB(CIEXYZ);
    v3 sRGB = LinearRGBTosRGB(LinearRGB);
    u32 Result = PackRGBA(sRGB);
    
    return(Result);
}

inline f32
sRGBToLinearRGB(f32 S)
{
    S = Clamp01(S);
    
    f32 Result;
    if(S <= 0.04045f)
    {
        Result = S / 12.92f;
    }
    else
    {
        Result = Pow(((S + 0.055f) / 1.055f), 2.4f);
    }
    
    Assert((0.0f <= Result) && (Result <= 1.0f));
    
    return(Result);
}

inline v3
UnpackRGBAToCIELAB(u32 U)
{
    v3 sRGB = UnpackRGBA(U);
    v3 LinearRGB = sRGBToLinearRGB(sRGB);
    v3 CIEXYZ = LinearRGBToCIEXYZ(LinearRGB);
    v3 CIELAB = CIEXYZToCIELAB(CIEXYZ);
    
    return(CIELAB);
}

#define PALETTIZE_MATH_H
#endif
