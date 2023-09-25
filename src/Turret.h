#ifndef TURRET_H
#define TURRET_H
#include "raylib.h"
#include "Utilities.h"

typedef struct
{
    Vector2 pos;
    
    //How much the gun part of the turret should
    //rotate in order to turn towards the player.
    float gunRotation;//(in radians)
    
    Circle hitCircle;
} Turret;

void initTurrets(void);

//The turrets turn to face the player, so they need 
//to know where the player is located.
void drawTurrets(void);

void unloadTurretTextures(void);

//The turrets turn to face the player, so they need 
//to know where the player is located.
void updateTurrets(const Vector2* playerPos, float dt);

void turretCollisionResolution(Index_t idxOfHitTurret);

//Returns a const ref to the turrets. Used by the collision detection function.
//outSize is filled in with the current number of active turrets.
const Turret* getTurretsConstRef(size_t* outSize);

#endif