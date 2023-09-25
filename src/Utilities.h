#ifndef UTILS_H
#define UTILS_H
#include "raylib.h"
#include <stdint.h>

//Contains random helper math functions and useful type aliases.

typedef uint64_t Index_t;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

typedef struct 
{
    Vector2 pos;
    float radius;
} Circle;

//Turns outVec into a Vector2 with a length of 1.
void normalizeVector2(Vector2* outVec);

float radians2Degrees(float radians);

//Returns a new vector which is vec rotated a given 
//amount of radians around rotationPoint.
Vector2 rotateVector2AroundPoint(const Vector2* vec, 
    const Vector2* rotationPoint, float radians);

bool isMouseOverRect(const Rectangle*);

#endif