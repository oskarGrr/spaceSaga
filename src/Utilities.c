#include "Utilities.h"
#include "windowSize.h"
#include "raylib.h"
#include "math.h"//sqrt(), cos(), sin()

//Contains random helper math functions and useful type aliases.

//Turns outVec into a Vector2 with a length of 1.
void normalizeVector2(Vector2* outVec)
{
    double magnitude = sqrt(outVec->x * outVec->x + outVec->y * outVec->y);
    outVec->x /= magnitude;
    outVec->y /= magnitude;
}

Vector2 rotateVector2AroundPoint(const Vector2* vec, 
    const Vector2* rotationPoint, float radians)
{
    //originPos is now the input vector, but translated to the origin.
    Vector2 originPos;
    originPos.x = vec->x - rotationPoint->x;
    originPos.y = vec->y - rotationPoint->y;
    
    //Rotate originPos by radians around the origin.
    Vector2 rotatedPos;
    float cosineOfAngle = cos(radians), sineOfAngle = sin(radians);
    rotatedPos.x = (originPos.x * cosineOfAngle) - (originPos.y * sineOfAngle);
    rotatedPos.y = (originPos.x * sineOfAngle)   + (originPos.y * cosineOfAngle);
    
    //Return the rotated vector translated back into the world.
    return (Vector2){rotationPoint->x + rotatedPos.x, rotationPoint->y + rotatedPos.y};
}

float radians2Degrees(float radians)
{
    return radians * 180.0f / PI;
}

bool isMouseOverRectV(Vector2 rectSize, Vector2 rectPos)
{
    Vector2 const mousePos = GetMousePosition();
    return mousePos.x > rectPos.x &&
           mousePos.x < rectPos.x + rectSize.x &&
           mousePos.y > rectPos.y &&
           mousePos.y < rectPos.y + rectSize.y;
}

bool isMouseOverRect(const Rectangle* rect)
{
    Vector2 mousePos = GetMousePosition();
    
    return 
    (
        mousePos.x > rect->x &&
        mousePos.x < rect->x + rect->width &&
        mousePos.y > rect->y &&
        mousePos.y < rect->y + rect->height
    );
}