
#pragma once
 
#include <cstddef>
#include <cassert>
#include <cfloat>
#include <cstring>
#include <cstdlib>
#include <assert.h>
#include <cstdio>
#include <cmath>

typedef int8_t    int8;
typedef int16_t   int16;
typedef int32_t   int32;

typedef u_int8_t  uint8;
typedef u_int16_t uint16;
typedef int32_t   uint32;
typedef int64_t   int64;

typedef float float32;
typedef double float64;

const float32 maxFloat = FLT_MAX;
const float32 epsilon = FLT_EPSILON;
const float32 pi = 3.14159265359f;

template <typename T>
inline T Max(T a, T b)
{
    return a > b ? a : b;
}


 
 



#if defined(_DEBUG)
#include <assert.h>
#define DEBUG_BREAK_IF(condition) if (condition) { printf("Debug break: %s at %s:%d\n", #condition, __FILE__, __LINE__); std::exit(EXIT_FAILURE); }
#else
#define DEBUG_BREAK_IF(_CONDITION_)
#endif



inline size_t CalculateCapacityGrow(size_t capacity, size_t minCapacity)
{
    if (capacity < minCapacity)
    capacity = minCapacity;
    if (capacity < 8)
    {
        capacity = 8;
    }
    else
    {
        // Round up to the next power of 2 and multiply by 2 (http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2)
        capacity--;
        capacity |= capacity >> 1;
        capacity |= capacity >> 2;
        capacity |= capacity >> 4;
        capacity |= capacity >> 8;
        capacity |= capacity >> 16;
        capacity++;
    }
    return capacity;
}

static inline size_t  GROW_CAPACITY(size_t capacity)
{
    return ((capacity) < 8 ? 8 : (capacity) * 2);
}