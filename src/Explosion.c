#include "Explosion.h"
#include "raylib.h"
#include "Utilities.h"
#include <stdbool.h>

//The max number of explosions that can be on the screen at one time.
//There will only be 1 at a time in reality, so this might be overkill.
#define ANIMATION_BUFF_SIZE 16

#define NUM_ANIMATION_FRAMES 9

typedef struct
{
    Vector2 pos;
    
    //Keeps track of which frame the animation is on 
    //(0 through NUM_ANIMATION_FRAMES - 1).
    u32 currentFrame;
    
    //Keeps track of how much time has passed since the last frame
    //of the explosion animation was played. The game runs at
    //60 fps but the explosion animation runs at 20 fps.
    float timer;
    
    bool isNewExplosion;
    
} ExplosionInfo;

static size_t sNumActiveExplosions = 0;
static ExplosionInfo sExplosionBuffer[ANIMATION_BUFF_SIZE];
static Texture2D sExplosionTexture;
static Sound sExplosionSound;

void initExplosionsResources(void)
{
    sExplosionTexture = LoadTexture("resources/textures/explosionTest.png");
    sExplosionSound = LoadSound("resources/sounds/explosion.wav");
    SetSoundVolume(sExplosionSound, 0.5f);
}

void releaseExplosionResources(void)
{
    UnloadTexture(sExplosionTexture);
    UnloadSound(sExplosionSound);
}

//Queues a new explosion animation at pos. pos will be the center of 
//where the explosion happens. After calling this to start a new explosion,
//call playAllActiveExplosions() to progress all current explosions by 1 frame.
void queueNewExplosionAnimation(const Vector2* pos)
{
    sExplosionBuffer[sNumActiveExplosions].pos = *pos;
    sExplosionBuffer[sNumActiveExplosions].currentFrame = 0;
    sExplosionBuffer[sNumActiveExplosions].timer = 0.0f;
    sExplosionBuffer[sNumActiveExplosions].isNewExplosion = true;
    
    ++sNumActiveExplosions;
}

//Should be called every frame inside of drawEverything() to progress  
//all of the currently active explosion animations by 1 frame.
void playAllActiveExplosionAnimations(float dt)
{
    //The animation will play at 20 fps.
    const float frameTime = 0.05;
    const float explosionWidth = sExplosionTexture.width / 9.0f;
    
    const size_t currentNumExplosions = sNumActiveExplosions;
    for(int i = 0; i < currentNumExplosions; ++i)
    {
        ExplosionInfo* ei = sExplosionBuffer + i;//shorter name
        
        if(ei->isNewExplosion)
        {
            PlaySound(sExplosionSound);
            ei->isNewExplosion = false;
        }         
        
        Rectangle textureRec =
        {
            .x = ei->currentFrame * explosionWidth,
            .y = 0.0f,
            .width = sExplosionTexture.width / 9,
            .height = sExplosionTexture.height
        };
        
        ei->timer += dt;
        if(ei->timer >= frameTime)
        {
            ei->timer = 0.0f;
            
            ++(ei->currentFrame);
            
            //if the explosion is over
            if(ei->currentFrame >= 9)
            {
                ei->currentFrame = 0;
                --sNumActiveExplosions;
            }
        }
        
        Vector2 texturePos;
        texturePos.x = ei->pos.x - explosionWidth * 0.5f;
        texturePos.y = ei->pos.y - sExplosionTexture.height * 0.5f;
        DrawTextureRec(sExplosionTexture, textureRec, texturePos, WHITE);     
    }
}