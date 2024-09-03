#ifndef TRAYRACING_VEC3_H
#define TRAYRACING_VEC3_H 

#define _USE_MATH_DEFINES
#include <math.h>
#include "defines.h"

typedef struct Vec3 {
    float x;
    float y;
    float z;
} Vec3;

static inline Vec3 inv(Vec3 a)
{
    Vec3 ret = {-a.x, -a.y, -a.z};
    return ret;
}

static inline Vec3 add(Vec3 a, Vec3 b)
{
    Vec3 ret = {a.x + b.x, a.y + b.y, a.z + b.z};
    return ret;
}

static inline Vec3 sub(Vec3 a, Vec3 b)
{
    Vec3 ret = {a.x - b.x, a.y - b.y, a.z - b.z};
    return ret;
}

static inline Vec3 mulf(float f, Vec3 a)
{
    Vec3 ret = {f * a.x, f * a.y, f * a.z };
    return ret;
}

static inline Vec3 mul(Vec3 a, Vec3 b)
{
    Vec3 ret = {a.x * b.x, a.y * b.y, a.z * b.z};
    return ret;
}

static inline float dot(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline Vec3 cross(Vec3 a, Vec3 b)
{
    Vec3 ret = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    return ret;
}

static inline float lengthSqr(Vec3 a)
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

static inline float length(Vec3 a)
{
    return sqrtf(lengthSqr(a));
}

static inline Vec3 norm(Vec3 a)
{
    float const len = length(a);

    return (len > PRECISION) ? mulf(1.0f / len, a) : a;
}

static inline Vec3 reflect(Vec3 n, Vec3 v) 
{
    return sub(v, mulf(2.0f,  mulf(dot(n, v),  n)));
}

static inline Vec3 refract(Vec3 n, Vec3 i)
{	
    return add(mulf((1.0f/n.x), i),  mulf((dot(n, i) / n.x - sqrtf(1.0f - (1.0f - powf(dot(n, i), 2)) / (n.x * n.x))), n));
}

#endif
