#include "raylib.h"
#include "Invader.h"
#include "windowSize.h"
#include "Utilities.h"
#include "Explosion.h"
#include "Bullet.h"
#include <stdint.h>
#include <assert.h>
#include <string.h>//memcpy()
#include <stdlib.h>//rand()

#define INVADER_BUFFSIZE 21
#define NUM_INVADER_TYPES 3
#define INITIAL_INVADER_VELOCITY 30.0f

static Invader sInvaderBuffer[INVADER_BUFFSIZE];
static size_t sNumAliveInvaders = INVADER_BUFFSIZE;
static float sShootCooldownTime = 0.8f;//The invaders shoot a bullet every .8 seconds.
static Texture2D sTextures[NUM_INVADER_TYPES];//invader textures
static float sInvaderVelocity = INITIAL_INVADER_VELOCITY;

void resetInvaders()
{
    sNumAliveInvaders = INVADER_BUFFSIZE;
    sInvaderVelocity = INITIAL_INVADER_VELOCITY;
    initInvaders();
}

//helper method for initInvaders()
//idxOfFrontRow is an index into sInvaderBuffer where the front row invaders should start from.
static Index_t initFrontRowInvaders(const Index_t idxOfFrontRow, const float yCoord)
{   
    const size_t numFrontRowInvaders = 6;
    const float pixelsBetweenInvaders =  WINDOW_WIDTH / (numFrontRowInvaders + 1);
    
    for(int i = idxOfFrontRow; i < numFrontRowInvaders; ++i)
    {
        sInvaderBuffer[i].pos = (Vector2){(i + 1) * pixelsBetweenInvaders, yCoord};
        sInvaderBuffer[i].type = FRONT_INVADER_TYPE;
    }
    
    return numFrontRowInvaders;
}

//helper method for initInvaders().
//idxOfMiddleRow is an index into sInvaderBuffer where the middle row invaders should start from.
static Index_t initMiddleRowInvaders(const Index_t idxOfMiddleRow, const float yCoord)
{   
    const size_t numMiddleRowInvaders = 7;
    const Index_t onePastEndIdx = numMiddleRowInvaders + idxOfMiddleRow;
    const float pixelsBetweenInvaders =  WINDOW_WIDTH / (numMiddleRowInvaders + 1);
    
    int j = 1;
    for(int i = idxOfMiddleRow; i < onePastEndIdx; ++i, ++j)
    {
        sInvaderBuffer[i].pos = (Vector2){j * pixelsBetweenInvaders, yCoord};
        sInvaderBuffer[i].type = MIDDLE_INVADER_TYPE;
    }
    
    return onePastEndIdx;
}

//helper method for initInvaders().
//idxOfBackRow is an index into sInvaderBuffer where the back row invaders should start from.
static void initBackRowInvaders(const Index_t idxOfBackRow, const float yCoord)
{
    const size_t numBackRowInvaders = 8;
    const Index_t onePastEndIdx = numBackRowInvaders + idxOfBackRow;
    const float pixelsBetweenInvaders =  WINDOW_WIDTH / (numBackRowInvaders + 1);
    
    int j = 1;
    for(int i = idxOfBackRow; i < onePastEndIdx; ++i, ++j)
    {
        sInvaderBuffer[i].pos = (Vector2){j * pixelsBetweenInvaders, yCoord};
        sInvaderBuffer[i].type = BACK_INVADER_TYPE;
    }
}

void initInvaders(void)
{
    //initInvaders() gets called more than once (if resetInvaders() gets called)
    //so check if we have already loaded the textures.
    static bool areTexturesInitialized = false;
    if( ! areTexturesInitialized )
    {
        sTextures[BACK_INVADER_TYPE]   = LoadTexture("resources/textures/BackInvader.png");
        sTextures[MIDDLE_INVADER_TYPE] = LoadTexture("resources/textures/MiddleInvader.png");
        sTextures[FRONT_INVADER_TYPE]  = LoadTexture("resources/textures/FrontInvader.png");
        areTexturesInitialized = true;
    }
    
    //Where the invaders start from (from the top of the window).
    float yCoordOfBackRow = WINDOW_HEIGHT * 0.16f;
    float yCoordOfMiddleRow = yCoordOfBackRow + sTextures[BACK_INVADER_TYPE].height + 20.0f;
    float yCoordOfFrontRow = yCoordOfMiddleRow + sTextures[MIDDLE_INVADER_TYPE].height + 20.0f;
    
    Index_t middleRowIdx = initFrontRowInvaders(0, yCoordOfFrontRow);
    Index_t backRowIdx = initMiddleRowInvaders(middleRowIdx, yCoordOfMiddleRow);
    initBackRowInvaders(backRowIdx, yCoordOfBackRow);
    
    for(int i = 0; i < INVADER_BUFFSIZE; ++i)
    {        
        float hitCircleXpos = -15.0f;
        for(int j = 0; j < INVADER_NUM_HITCIRCLES; ++j, hitCircleXpos += 15.0f)
        {
            sInvaderBuffer[i].hitCircles[j].radius = 8.0f;
            sInvaderBuffer[i].hitCircles[j].pos.x = sInvaderBuffer[i].pos.x + hitCircleXpos;
            sInvaderBuffer[i].hitCircles[j].pos.y = sInvaderBuffer[i].pos.y;
        }
    }
}

void drawInvaders(void)
{
    //shorter names
    Texture2D* it = sTextures;
    Invader* ib = sInvaderBuffer;
    
    for(int i = 0; i < sNumAliveInvaders; ++i)
    {
        Vector2 texturePosition;
        texturePosition.x = ib[i].pos.x - it[ib[i].type].width * 0.5f;
        texturePosition.y = ib[i].pos.y - it[ib[i].type].height * 0.5f;
        DrawTextureV(it[ib[i].type], texturePosition, WHITE); 
#ifndef NDEBUG
        for(int j = 0; j < INVADER_NUM_HITCIRCLES; ++j)
        {
            DrawCircleLines(ib[i].hitCircles[j].pos.x, 
                ib[i].hitCircles[j].pos.y, ib[i].hitCircles[j].radius, BLUE);
        }
#endif
    }
}

//Returns true if the last invader was eliminated.
bool invaderCollisionResolution(Index_t idxOfHitInvader)
{
    assert(idxOfHitInvader < sNumAliveInvaders);
    
    queueNewExplosionAnimation(&sInvaderBuffer[idxOfHitInvader].pos);
    
    //If the invader that was hit was the last invader in the array,  
    //then don't memcpy since the objects overlap which leads to UB.
    if(idxOfHitInvader != sNumAliveInvaders - 1)
    {
        memcpy(sInvaderBuffer + idxOfHitInvader,
            sInvaderBuffer + sNumAliveInvaders - 1, sizeof(Invader));
    }
    
    const float shootSpeedIncreaseAmount = 0.04f;
    sShootCooldownTime += shootSpeedIncreaseAmount;
    
    const float speedMultiplier = 1.18f;
    sInvaderVelocity *= speedMultiplier;
    
    --sNumAliveInvaders;
    
    return sNumAliveInvaders == 0;
}

//get the lowest invader's Y coord (the bottom of the texture).
float getLowestTextureYCoord(void)
{
    float lowest = 0.0f;
    for(int i = 0; i < sNumAliveInvaders; ++i)
    {
        float const halfTextureHeight = sTextures[sInvaderBuffer[i].type].height * 0.5f;
        if(sInvaderBuffer[i].pos.y + halfTextureHeight > lowest)
            lowest = sInvaderBuffer[i].pos.y + halfTextureHeight;
    }

    return lowest;
}

//finds the index of the invader with the largest x coord
static Index_t findMaxInvader(void)
{
    float biggestXcoord = 0.0f;
    Index_t idxOfBiggestXcoord = 0;
    
    for(int i = 0; i < sNumAliveInvaders; ++i)
    {
        if(sInvaderBuffer[i].pos.x > biggestXcoord)
        {
            biggestXcoord = sInvaderBuffer[i].pos.x;
            idxOfBiggestXcoord = i;
        }
    }
    
    return idxOfBiggestXcoord;
}

//finds the index of the invader with the smallest x coord
static Index_t findMinInvader(void)
{
    float smallestXcoord = WINDOW_WIDTH;
    Index_t idxOfSmallestXcoord = 0;
    
    for(int i = 0; i < sNumAliveInvaders; ++i)
    {
        if(sInvaderBuffer[i].pos.x < smallestXcoord)
        {
            smallestXcoord = sInvaderBuffer[i].pos.x;
            idxOfSmallestXcoord = i;
        }
    }
    
    return idxOfSmallestXcoord;
}

//Check if an invader has moved too far to the left or right.
static bool isInvaderOnScreen(bool isGoingRight, const Invader* 
    invader, float invaderWidth)
{
    if(isGoingRight) {return invader->pos.x + invaderWidth * 0.5f < WINDOW_WIDTH;}
    return invader->pos.x - invaderWidth * 0.5f > 0.0f;
}

//Helper function to reduce updateInvaders() size.
//Pics a random invader to shoot at the player.
static void shootInvaderBulletHelper(float dt)
{
    static float timer = 0.0f;
    timer += dt;
    if(timer > sShootCooldownTime)
    {
        Index_t randInvader = rand() % sNumAliveInvaders;
        Vector2 bulletPos = sInvaderBuffer[randInvader].pos;
        bulletPos.y += sTextures[sInvaderBuffer[randInvader].type].height * 0.5f;
        Vector2 downVec = {0.0f, 1.0f};
        shootBullet(INVADER_BULLET, &bulletPos, &downVec, 0.0f);
        timer = 0.0f;
    }
}

void updateInvaders(float dt)
{
    //TODO check if all invaders are dead
    
    static bool direction = false;//false == left
    
    //Whether or not the invaders are in their lowering phase.
    static bool isCurrentlyLowering = false;
    
    //If we are going right then find the right most invader, otherwise find the left most.
    Index_t rightOrLeftMost = direction ? findMaxInvader() : findMinInvader();
    float rightOrLeftMostWidth = sTextures[sInvaderBuffer[rightOrLeftMost].type].width;
    
    if( ! isInvaderOnScreen(direction, sInvaderBuffer + rightOrLeftMost, rightOrLeftMostWidth) )
    {
        direction = !direction;
        isCurrentlyLowering = true;
    }
    
    for(int i = 0; i < sNumAliveInvaders; ++i)
    {
        if(direction)//If the invaders are going right.
        {
            sInvaderBuffer[i].pos.x += sInvaderVelocity * dt;
            for(int j = 0; j < INVADER_NUM_HITCIRCLES; ++j)
                sInvaderBuffer[i].hitCircles[j].pos.x += sInvaderVelocity * dt;            
        }
        else
        {
            sInvaderBuffer[i].pos.x -= sInvaderVelocity * dt;
            for(int j = 0; j < INVADER_NUM_HITCIRCLES; ++j)
                sInvaderBuffer[i].hitCircles[j].pos.x -= sInvaderVelocity * dt;
        }
    }
    
    if(isCurrentlyLowering)
    {
        //How many pixels to lower the invaders in their lowering phase.
        const float lowerAmount = 22.0f;
        
        //Keep track of how much we have lowered durring the lowering phase.
        static float loweringCount = 0.0f;
        
        for(int i = 0; i < sNumAliveInvaders; ++i)
        {      
            sInvaderBuffer[i].pos.y += sInvaderVelocity * dt;
            for(int j = 0; j < INVADER_NUM_HITCIRCLES; ++j)
                sInvaderBuffer[i].hitCircles[j].pos.y += sInvaderVelocity * dt;
        }
        
        loweringCount += sInvaderVelocity * dt;
        if(loweringCount > lowerAmount)
        {
            loweringCount = 0.0f;
            isCurrentlyLowering = false;
        }
    }
    
    shootInvaderBulletHelper(dt);//Helper function to reduce size.
}

void unloadInvaderTextures(void)
{
    for(int i = 0; i < NUM_INVADER_TYPES; ++i)
        UnloadTexture(sTextures[i]);
}

size_t getNumAliveInvaders(void)
{
    return sNumAliveInvaders;
}

//Returns a const reference to the array of hitCircle's for a given index 
//into the array of invaders. The number of hit circles in this array is
//defined as the macro INVADER_NUM_HITCIRCLES in invader.h. All invaders have the
//same number of hit circles. The current number of alive invaders
//in the array is retrieved with the function getNumAliveInvaders().
const Circle* getHitCircles(Index_t whichInvader)
{
    return sInvaderBuffer[whichInvader].hitCircles;
}