#include "raylib.h"
#include "Bullet.h"
#include "stddef.h"
#include "Utilities.h"
#include "windowSize.h"
#include "stdbool.h"
#include <string.h>//memcpy()
#include <assert.h>

#define NUM_BULLET_TYPES 3
#define BULLET_BUFF_CAPACITY 32
#define NUM_BULLET_TEXTURES NUM_BULLET_TYPES

static Bullet sBulletBuffer[BULLET_BUFF_CAPACITY];
static Texture2D sBulletTextures[NUM_BULLET_TEXTURES]; 
static size_t sNumActiveBullets = 0;

void initBullets(void)
{
    sBulletTextures[PLAYER_BULLET]  = LoadTexture("resources/textures/playerBullet.png");
    sBulletTextures[INVADER_BULLET] = LoadTexture("resources/textures/invaderBullet.png");
    sBulletTextures[TURRET_BULLET]  = LoadTexture("resources/textures/turretBullet.png");
}

static bool isBulletOnScreen(Index_t idx)
{
    const Bullet* this = sBulletBuffer + idx;
    
    if(this->type == PLAYER_BULLET)
    {
        const float halfLength = sBulletTextures[PLAYER_BULLET].height * 0.5f;
        return this->pos.y + halfLength > 0;
    }
    
    const float length = this->type == INVADER_BULLET ? 
        sBulletTextures[INVADER_BULLET].height : sBulletTextures[TURRET_BULLET].height;
    const float halfLength = length * 0.5f;
    
    return this->pos.y - halfLength < WINDOW_HEIGHT;
}

static void eraseBullet(Index_t idxOfBulletToErase)
{
    assert(idxOfBulletToErase < sNumActiveBullets);
    
    //If the bullet that was hit was the last bullet in the array,  
    //then don't memcpy since the objects overlap which leads to UB.
    if(idxOfBulletToErase != sNumActiveBullets - 1)
    {
        memcpy(sBulletBuffer + idxOfBulletToErase, 
            sBulletBuffer + sNumActiveBullets - 1, sizeof(Bullet));
    }  
    
    --sNumActiveBullets;
}

//Called from detectCollisions() to notify the bullets that the 
//bullet at index "idx" collided with an entity.
void bulletCollisionResolution(Index_t idx)
{
    eraseBullet(idx);
}

//BulletType is defined in Bullet.h as either an invader bullet, a player bullet, or 
//a turret bullet. newPos and newDir are the position and direction of the bullet.
//newDir should be a normal vector. newPos should be the middle of where the bullet will go.
void shootBullet(BulletType bulletType, const Vector2* pos, 
    const Vector2* newDir, float rotation)
{
    //BULLET_BUFF_CAPACITY is large enough to hold way more bullets 
    //than will ever be on the screen. I threw this condition here 
    //just to be sure even though it's not really needed.
    if(sNumActiveBullets >= BULLET_BUFF_CAPACITY)
        return;
    
    sBulletBuffer[sNumActiveBullets].pos = *pos;
    sBulletBuffer[sNumActiveBullets].type = bulletType;
    sBulletBuffer[sNumActiveBullets].hitCircle.pos = *pos;
    sBulletBuffer[sNumActiveBullets].direction = *newDir;
    sBulletBuffer[sNumActiveBullets].rotation = rotation;
    
    if(bulletType == PLAYER_BULLET)
        sBulletBuffer[sNumActiveBullets].hitCircle.radius = 10.0f;
    else if(bulletType == INVADER_BULLET)
        sBulletBuffer[sNumActiveBullets].hitCircle.radius = 6.0f;
    else//if bulletType == TURRET_BULLET
        sBulletBuffer[sNumActiveBullets].hitCircle.radius = 8.0f;
    
    ++sNumActiveBullets;
}

//Moves the bullets by one "step" in their correct direction.
void updateBulletPositions(float dt)
{
    const float velocity = 700.0f;
    
    for(int i = 0; i < sNumActiveBullets; ++i)
    {
        sBulletBuffer[i].pos.x += sBulletBuffer[i].direction.x * velocity * dt;
        sBulletBuffer[i].pos.y += sBulletBuffer[i].direction.y * velocity * dt;
        sBulletBuffer[i].hitCircle.pos.x += sBulletBuffer[i].direction.x * velocity * dt;
        sBulletBuffer[i].hitCircle.pos.y += sBulletBuffer[i].direction.y * velocity * dt;
        
        if(!isBulletOnScreen(i))
            eraseBullet(i);
    }
}

//Draw all the active bullets.
void drawBullets(void)
{
    //shorter names
    Bullet* bb = sBulletBuffer;
    Texture2D* bt = sBulletTextures;
    
    for(int i = 0; i < sNumActiveBullets; ++i)
    {
        Vector2 texturePos;
        texturePos.x = bb[i].pos.x - bt[bb[i].type].width * 0.5f;
        texturePos.y = bb[i].pos.y - bt[bb[i].type].height * 0.5f;
        
        if(bb[i].type == TURRET_BULLET)
        {
            texturePos = rotateVector2AroundPoint(&texturePos, &bb[i].pos, bb[i].rotation);
            DrawTextureEx(bt[bb[i].type], texturePos, 
                radians2Degrees(bb[i].rotation), 1.0f, WHITE);
        }
        else {DrawTextureV(bt[bb[i].type], texturePos, WHITE);}
    }
}

void unloadBulletTextures(void)
{
    for(int i = 0; i < NUM_BULLET_TEXTURES; ++i)
        UnloadTexture(sBulletTextures[i]);
}

//Returns a const ref to the bullets. Used by the collision detection function.
//outSize is filled in with the current number of active bullets.
const Bullet* getBulletsConstRef(size_t* outSize)
{
    *outSize = sNumActiveBullets;
    return sBulletBuffer;
}