#ifndef BULLET_H
#define BULLET_H
#include "Utilities.h"
#include <stddef.h>

typedef struct Vector2 Vector2;

//There are three different types of bullets depending on who shot the bullet.
typedef enum
{
    PLAYER_BULLET, 
    INVADER_BULLET, 
    TURRET_BULLET
} BulletType;

typedef struct
{
    Vector2 pos;//position will be the middle of the bullet
    Vector2 direction;//unit length direction vector
    float rotation;//in radians
    Circle hitCircle;
    BulletType type;
} Bullet;

void initBullets(void);

void bulletCollisionResolution(Index_t idx);

//BulletType is defined in Bullet.h as either an invader bullet, a player bullet, or 
//a turret bullet. newPos and newDir are the position and direction of the bullet.
//newDir should be a normal vector. newPos should be the middle of where the bullet will go.
void shootBullet(BulletType bulletType, const Vector2* newPos, 
    const Vector2* newDir, float rotation);

//Moves the bullets by one "step" in their correct direction.
void updateBulletPositions(float dt);

void drawBullets(void);

void unloadBulletTextures(void);

//Returns a const ref to the bullets. Used by the collision detection function.
//outSize is filled in with the current number of active bullets.
const Bullet* getBulletsConstRef(size_t* outSize);

void resetBullets(void);

#endif