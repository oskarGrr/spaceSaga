#ifndef COLLISION_H
#define COLLISION_H
#include "raylib.h"
#include "Utilities.h"
#include <stdbool.h>

//Returned as an out paramater from detectCollisions().
typedef struct
{
    //Did the player, or a turret, or an invader get hit.
    enum {PLAYER_WAS_HIT, INVADER_WAS_HIT, TURRET_WAS_HIT} typeOfEntityThatWasHit;
    
    //The index into the respective container of entities (if the entity that was hit
    //was the player this number will just be 0).
    Index_t idxOfEntityThatWasHit;
    
    //The index of the bullet that collided with the entity.
    Index_t idxOfBullet;
} CollisionInfo;

//Detects collisions between enemies and the player's bullets. Also detects collisions between
//the enemy bullets and the player. outCollisionInfo is filled in as an out param, and
//a bool is returned to determine whether or not a collision happened.
bool detectCollisions(CollisionInfo* outCollisionInfo);

#endif