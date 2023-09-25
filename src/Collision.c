#include "Collision.h"
#include "Invader.h"
#include "raylib.h"
#include "Utilities.h"
#include "Bullet.h"
#include "Turret.h"

static bool areCirclesColliding(const Circle* c0, const Circle* c1)
{
    Vector2 c0ToC1 = {c1->pos.x - c0->pos.x, c1->pos.y - c0->pos.y};
    float magnitudeSqrd = c0ToC1.x * c0ToC1.x + c0ToC1.y * c0ToC1.y;
    float radiusSumSqrd = (c0->radius + c1->radius) * (c0->radius + c1->radius);
    return radiusSumSqrd >= magnitudeSqrd;
}

//Returns the index of which invader was hit. Returns -1 if an invader was not hit.
static int checkPlayerBulletVsInvaders(const Bullet* playerBullet)
{
    size_t numAliveInvaders = getNumAliveInvaders();
    
    //For each invader.
    for(int i = 0; i < numAliveInvaders; ++i)
    {
        //For each hit circle in each invader.
        for(int j = 0; j < INVADER_NUM_HITCIRCLES; ++j)
        {
            const Circle* hitCircles = getHitCircles(i);
            if(areCirclesColliding(&hitCircles[j], &playerBullet->hitCircle))
                return i;
        }
    }
    
    return -1;
}

//Returns the index of which turret was hit. Returns -1 if a turret was not hit.
static int checkPlayerBulletVsTurrets(const Bullet* playerBullet)
{
    size_t numActiveTurrets = 0;
    const Turret* turrets = getTurretsConstRef(&numActiveTurrets);
    
    //For each turret.
    for(int i = 0; i < numActiveTurrets; ++i)
    {
        if(areCirclesColliding(&turrets[i].hitCircle, &playerBullet->hitCircle))
            return i;
    }
    
    return -1;
}

//Detects collisions between enemies and the player's bullets. Also detects collisions between
//the enemy bullets and the player. outCollisionInfo is filled in as a CollisionInfo 
//structure (defined in Collision.h), and contains information about the collision.
//A bool is also returned to determine whether or not a collision happened at all.
bool detectCollisions(CollisionInfo* outCollisionInfo)
{
    size_t numActiveBullets = 0;
    const Bullet* bullets = getBulletsConstRef(&numActiveBullets);
    
    for(int i = 0; i < numActiveBullets; ++i)
    {
        if(bullets[i].type == PLAYER_BULLET)
        {
            //Type needs to be signed since the return can be -1
            //to indicate that no collision happened.
            int idxOfInvaderThatWasHit = checkPlayerBulletVsInvaders(bullets + i);
            
            //If an invader was hit by a player bullet.
            if(idxOfInvaderThatWasHit >= 0)
            {
                outCollisionInfo->typeOfEntityThatWasHit = INVADER_WAS_HIT;
                outCollisionInfo->idxOfEntityThatWasHit = idxOfInvaderThatWasHit;
                outCollisionInfo->idxOfBullet = i;
                return true;
            }
            
            //Type needs to be signed since the return can be -1
            //to indicate that no collision happened.
            int idxOfTurretThatWasHit = checkPlayerBulletVsTurrets(bullets + i);
            
            //If a turret was hit by a player bullet.
            if(idxOfTurretThatWasHit >= 0)
            {
                outCollisionInfo->typeOfEntityThatWasHit = TURRET_WAS_HIT;
                outCollisionInfo->idxOfEntityThatWasHit = idxOfTurretThatWasHit;
                outCollisionInfo->idxOfBullet = i;
                return true;
            }
        }
        else//if type == INVADER_BULLET || type == TURRET_BULLET
        {
            //checkEnemyBulletVsPlayer();
        }
    }
    
    return false;
}