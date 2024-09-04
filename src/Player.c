#include "raylib.h"
#include "Player.h"
#include "Utilities.h"
#include "windowSize.h"
#include <assert.h>

void resetPlayer(Player* this)
{
    playerCtor(this);
}

void playerCtor(Player* this)
{
    //resetPlayer() simply calls playerCtor() again, 
    //so check to make sure we don't load the texture more than once.
    static bool isTextureLoaded = false;
    if( ! isTextureLoaded )
    {
        this->texture = LoadTexture("resources/textures/playerShip.png");
        isTextureLoaded = true;
    }

    this->position = (Vector2){WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.85f};
    this->numLives = 3;
    this->velocity = 420.0f;
    this->shootCooldownTime = 0.65f;
    
    //place all the hit circles in their initial positions so that they are spaced out correctly.
    float const hitCircleRadius = (this->texture.width / (float)PLAYER_NUM_HIT_CIRCLES) / 2;
    float hitCircleXPos = (this->position.x - this->texture.width / 2.0f) + hitCircleRadius; 
    for(int i = 0; i < PLAYER_NUM_HIT_CIRCLES; ++i)
    {
        this->hitCircles[i].radius = hitCircleRadius;
        this->hitCircles[i].pos.x = hitCircleXPos;
        hitCircleXPos += hitCircleRadius * 2;
        this->hitCircles[i].pos.y = this->position.y;
    }

    //now that the hit circles are initialized at evenly spaced out
    //locations it's time to make a few adjustments...
    //If built in debug mode you can see where the hit circles are.

#if PLAYER_NUM_HIT_CIRCLES & 1 //if PLAYER_NUM_HIT_CIRCLES is odd
    
    this->hitCircles[PLAYER_NUM_HIT_CIRCLES / 2].pos.y -= this->texture.height * 0.4f;
    this->hitCircles[PLAYER_NUM_HIT_CIRCLES / 2].radius *= 0.75f;

#else //if PLAYER_NUM_HIT_CIRCLES is even

    int const middleIdxRight = PLAYER_NUM_HIT_CIRCLES / 2;
    int const middleIdxLeft = middleIdxRight - 1;
    assert(middleIdxLeft >= 0);

    this->hitCircles[middleIdxRight].pos.x = this->position.x;
    this->hitCircles[middleIdxRight].pos.y -= this->texture.height * 0.5;
    this->hitCircles[middleIdxRight].radius *= 0.8f;

    this->hitCircles[middleIdxLeft].pos.x = this->position.x;
    this->hitCircles[middleIdxLeft].pos.y -= this->texture.height * 0.3;
    this->hitCircles[middleIdxLeft].radius *= 1.2f;  

#endif // PLAYER_NUM_HIT_CIRCLES & 1

    //Make the left most and right most hit circle a little smaller.
    this->hitCircles[0].radius *= 0.6f;
    this->hitCircles[PLAYER_NUM_HIT_CIRCLES - 1].radius *= 0.6f;

    //Move the left most and right most hit circle down the screen a bit. 
    this->hitCircles[0].pos.y += this->texture.height * 0.12f;
    this->hitCircles[PLAYER_NUM_HIT_CIRCLES - 1].pos.y += this->texture.height * 0.12f;
}

bool playerCollisionResolution(Player* this)
{
    --this->numLives;
    return this->numLives < 0;
}

void playerPositionUpdate(Player* this, LeftOrRight leftOrRight, float dt)
{
    float const modulatedVelocity = this->velocity * dt;
    
    if(leftOrRight == RIGHT)
    {
        //If we haven't reached the right side of the window.
        if(this->position.x + (this->texture.width * 0.5f) < WINDOW_WIDTH)
        {
            this->position.x += modulatedVelocity;//Then take a step to the right.
            for(int i = 0; i < PLAYER_NUM_HIT_CIRCLES; ++i) 
                this->hitCircles[i].pos.x += modulatedVelocity;
        }
    }
    else if(leftOrRight == LEFT)
    {
        //If we haven't reached the left side of the window.
        if(this->position.x - (this->texture.width * 0.5f) > 0)
        {
            this->position.x -= modulatedVelocity;//Then take a step to the left.
            for(int i = 0; i < PLAYER_NUM_HIT_CIRCLES; ++i) 
                this->hitCircles[i].pos.x -= modulatedVelocity;
        }
    }
}

void drawPlayer(const Player* this)
{
    Vector2 playerTexturePos;
    playerTexturePos.x = this->position.x - this->texture.width * 0.5f;
    playerTexturePos.y = this->position.y - this->texture.width * 0.5f;
    DrawTextureV(this->texture, playerTexturePos, WHITE);

#ifndef NDEBUG
    for(int i = 0; i < PLAYER_NUM_HIT_CIRCLES; ++i)
        DrawCircleLinesV(this->hitCircles[i].pos, this->hitCircles[i].radius, RED);
#endif
}