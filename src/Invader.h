#ifndef INVADER_H
#define INVADER_H
#include "raylib.h"
#include "Utilities.h"
#include <stddef.h>
#include <stdbool.h>

#define INVADER_NUM_HITCIRCLES 3

//There are three different invader types that only differ by texture.
//All of the invaders in a given row look the same and there are three rows.
//This enum will tell which row the invader is in,
//and which texture should be used to draw it.
typedef enum
{
    FRONT_INVADER_TYPE, 
    MIDDLE_INVADER_TYPE, 
    BACK_INVADER_TYPE
} InvaderType;

typedef struct
{
    Vector2 pos;
    InvaderType type;
    Circle hitCircles[INVADER_NUM_HITCIRCLES];
} Invader;

size_t getNumAliveInvaders(void);

//Returns a const reference to the array of hitCircle's for a given index 
//into the array of invaders. The number of hit circles in this array is
//defined as the macro INVADER_NUM_HITCIRCLES in invader.h. All invaders have the
//same number of hit circles. The current number of alive invaders
//in the array is retrieved with the function getNumAliveInvaders().
const Circle* getHitCircles(Index_t whichInvader);

//Returns true if the last invader was eliminated.
bool invaderCollisionResolution(Index_t idxOfHitInvader);

void drawInvaders(void);
void initInvaders(void);
void unloadInvaderTextures(void);
void updateInvaders(float dt);

#endif