#include "Turret.h"
#include "Utilities.h"
#include "windowSize.h"
#include "Bullet.h"
#include "Explosion.h"
#include <stddef.h>
#include <math.h>//atan2()
#include <stdlib.h>//rand()
#include <string.h>//memcpy()
#include <assert.h>

//number of initial turrets
#define TURRET_ARRAY_CAPACITY 4

static Texture2D sGunTexture, sBaseTexture;
static size_t sNumAliveTurrets = TURRET_ARRAY_CAPACITY;
static Turret sTurretBuffer[TURRET_ARRAY_CAPACITY];

void initTurrets(void)
{
    sGunTexture = LoadTexture("resources/textures/turretGun.png");
    sBaseTexture = LoadTexture("resources/textures/turretBase.png");
    
    const float pixelsBetween = WINDOW_WIDTH / (TURRET_ARRAY_CAPACITY + 1);
    const float pixelsFromTopOfWindow = WINDOW_HEIGHT * 0.07f;
    
    for(int i = 0; i < TURRET_ARRAY_CAPACITY; ++i)
    {
        sTurretBuffer[i].pos.x = pixelsBetween * (i + 1);
        sTurretBuffer[i].pos.y = pixelsFromTopOfWindow;
        sTurretBuffer[i].gunRotation = 0.0f;
        sTurretBuffer[i].hitCircle.radius = sBaseTexture.width * 0.5f;
        sTurretBuffer[i].hitCircle.pos = sTurretBuffer[i].pos;
    }
}

//The turrets turn to face the player, so they need 
//to know where the player is located.
void updateTurrets(const Vector2* playerPos, float dt)
{
    Turret* tb = sTurretBuffer;//shorter name
    
    if(sNumAliveTurrets <= 0) 
        return;
    
    for(int i = 0; i < sNumAliveTurrets; ++i)
    { 
        Vector2 turret2Player = (Vector2){playerPos->x - tb[i].pos.x, 
            playerPos->y - tb[i].pos.y};
        
        tb[i].gunRotation = atan2(turret2Player.y, turret2Player.x) - (PI / 2.0f); 
    }
    
    //The turrets shoot a bullet every 3.8 seconds.
    const float shootCooldownTime = 3.8f;
    static float shootTimer = 0.0f;
    shootTimer += dt;
    if(shootTimer >= shootCooldownTime)
    {
        //Pick a random turret out of the turrets that are alive to shoot.
        Index_t randIdx = rand() % sNumAliveTurrets;
        
        //The bullet direction will be a vector pointing from the turret to the player.
        Vector2 bulletDir = {playerPos->x - tb[randIdx].pos.x, 
            playerPos->y - tb[randIdx].pos.y};
        
        //Direction needs to have a length of 1 before being passed to shootBullet().
        normalizeVector2(&bulletDir);
        
        //The bullets spawn point will be in front of the gun barrels.
        Vector2 bulletPos = tb[randIdx].pos;
        bulletPos.y += sBaseTexture.height * 0.5f;
        bulletPos = rotateVector2AroundPoint(&bulletPos, &tb[randIdx].pos, 
            tb[randIdx].gunRotation);       
        
        shootBullet(TURRET_BULLET, &bulletPos, &bulletDir, tb[randIdx].gunRotation);
        
        shootTimer = 0.0f;
    }
}

void turretCollisionResolution(Index_t idxOfHitTurret)
{   
    assert(idxOfHitTurret < sNumAliveTurrets);
    
    queueNewExplosionAnimation(&sTurretBuffer[idxOfHitTurret].pos);
    
    //If the turret that was hit was the last turret in the array,  
    //then don't memcpy since the objects overlap which leads to UB.
    if(idxOfHitTurret != sNumAliveTurrets - 1)
    {
        memcpy(sTurretBuffer + idxOfHitTurret, 
            sTurretBuffer + sNumAliveTurrets - 1, sizeof(Turret));
    }
    
    --sNumAliveTurrets;
}

void drawTurrets(void)
{
    if(sNumAliveTurrets <= 0)
        return;
    
    const Turret* tb = sTurretBuffer;//shorter name
    
    for(int i = 0; i < sNumAliveTurrets; ++i)
    {
        //Draw the base portion of the turret.
        Vector2 turretBaseTexturePos;
        turretBaseTexturePos.x = tb[i].pos.x - sBaseTexture.width * 0.5f;
        turretBaseTexturePos.y = tb[i].pos.y - sBaseTexture.height * 0.5f;
        DrawTextureEx(sBaseTexture, turretBaseTexturePos, 0.0f, 1.0f, WHITE);
        
        //Draw the gun portion of the turret.
        Vector2 gunTexturePos;
        gunTexturePos.x = tb[i].pos.x - sGunTexture.width * 0.5f;
        gunTexturePos.y = tb[i].pos.y - sGunTexture.height * 0.5f;
        gunTexturePos = rotateVector2AroundPoint(&gunTexturePos, 
            &tb[i].pos, tb[i].gunRotation);
            
        DrawTextureEx(sGunTexture, gunTexturePos,
            radians2Degrees(tb[i].gunRotation), 1.0f, WHITE);
        
        DrawCircleLines(tb[i].pos.x, tb[i].pos.y, tb[i].hitCircle.radius, WHITE);
    }
}

void unloadTurretTextures(void)
{
    UnloadTexture(sGunTexture);
    UnloadTexture(sBaseTexture);
}

//Returns a const ref to the turrets. Used by the collision detection function.
//outSize is filled in with the current number of active turrets.
const Turret* getTurretsConstRef(size_t* outSize)
{
    *outSize = sNumAliveTurrets;
    return sTurretBuffer;
}