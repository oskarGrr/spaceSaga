#include <time.h> //time()
#include <stdlib.h> //srand()
#include <stdio.h>
#include "raylib.h"
#include "Player.h"
#include "windowSize.h"
#include "Bullet.h"
#include "Invader.h"
#include "Collision.h"
#include "Utilities.h"
#include "Explosion.h"
#include "Turret.h"

static void gameLoop(void);

//Helper functions to reduce gameLoop()'s size.
static void handleInput(Player*, float dt);
static int drawEverything(const Player* player, Texture2D* background, bool endPopupIsOpen);
static bool detectCollisionsHelper(Player* player);//Returns true if all the invaders have been eliminated.

int main(void)
{
    srand((unsigned)time(NULL));
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Space Saga");
    InitAudioDevice();
    SetTargetFPS(60);
    
    gameLoop();
    
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

static void gameLoop(void)
{
    initExplosionsResources();
    initBullets();
    initInvaders();
    initTurrets();
    Player player;
    playerCtor(&player);
    Texture2D mainBackground = LoadTexture("resources/textures/mainGameBackground.png");
    
    bool isGameOver = false;
    while( ! WindowShouldClose() )
    {
        int endPopupResult = drawEverything(&player, &mainBackground, isGameOver);

        if( ! isGameOver )
        {
            float dt = GetFrameTime();
            handleInput(&player, dt);
            updateBulletPositions(dt);
            updateInvaders(dt);
            updateTurrets(&player.position, dt);
            
            //If all the invaders have been elmininated or if the player has exhausted all of their lives.
            if(detectCollisionsHelper(&player)) { isGameOver = true; }
        }
        else
        {
            if(endPopupResult == -1) { break; } //quit button was pressed
            else if(endPopupResult == 1) //reset button was pressed
            {
                //resetGame();
                isGameOver = false;
            }
        }
    }
    
    UnloadTexture(player.texture);
    UnloadTexture(mainBackground);
    unloadBulletTextures();
    unloadInvaderTextures();
    unloadTurretTextures();
    releaseExplosionResources();
}

//Helper func to reduce gameLoop()'s size.
//Returns true if all the invaders have been eliminated or if
//the player has exhausted all of their lives.
static bool detectCollisionsHelper(Player* p)
{
    CollisionInfo collisionInfo;
    bool areAllInvadersDead = false;
    bool isPlayerDead = false;

    while(detectCollisions(&collisionInfo, p))
    {
        if(collisionInfo.typeOfEntityThatWasHit == PLAYER_WAS_HIT)
        {
            isPlayerDead = playerCollisionResolution(p);
        }

        else if(collisionInfo.typeOfEntityThatWasHit == INVADER_WAS_HIT)
            areAllInvadersDead = invaderCollisionResolution(collisionInfo.idxOfEntityThatWasHit);
        else//If a turret was hit.
            turretCollisionResolution(collisionInfo.idxOfEntityThatWasHit);
        
        bulletCollisionResolution(collisionInfo.idxOfBullet);
    }
    
    return areAllInvadersDead || isPlayerDead;
}

//Helper func to reduce gameLoop()'s size.
static void handleInput(Player* player, float dt)
{
    if(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        playerPositionUpdate(player, LEFT, dt);
    else if(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        playerPositionUpdate(player, RIGHT, dt);
    
    //There is a cooldown (of length player->shootCooldownTime in seconds)
    //on how fast the player can shoot bullets.
    static float timer = 0.0f;
    timer += dt;
    if(timer > player->shootCooldownTime)
    {
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 upVector = {0.0f, -1.0f};
            shootBullet(PLAYER_BULLET, &player->position, &upVector, 0.0f);
            timer = 0.0f;
        }
    }
}

//returns -1 if user presses the quit button.
//returns 1 if the user presses the reset button.
//returns 0 if the user has not yet pressed the reset or quit button.
int drawEndPopup(void)
{
    Vector2 const popupSize = {WINDOW_WIDTH / 5.0f, WINDOW_HEIGHT / 5.0f};
    Vector2 const popupPos = {WINDOW_WIDTH / 2 - popupSize.x / 2, WINDOW_HEIGHT / 2 - popupSize.y / 2};
    
    Vector2 const buttonSize = {popupSize.x * .8f, popupSize.y / 3};
    Vector2 const quitPos = {popupPos.x + popupSize.x * .1f, popupPos.y + buttonSize.y * 0.33f};
    Vector2 const resetPos = {quitPos.x, popupPos.y + buttonSize.y * 1.66f};
    
    char const* resetTxt = "Reset";
    char const* quitTxt = "Quit";
    float const fontSize = 22.0f;
    int const resetTxtSize = MeasureText(resetTxt, fontSize);
    int const quitTxtSize = MeasureText(quitTxt, fontSize);
    
    bool const isMouseOverReset = isMouseOverRectV(buttonSize, resetPos);
    bool const isMouseOverQuit = isMouseOverRectV(buttonSize, quitPos);
    
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if(isMouseOverReset) { return 1; }
        else if(isMouseOverQuit) { return -1; }
    }
    
    Color const quitColor = isMouseOverQuit ? GRAY : LIGHTGRAY;
    Color const resetColor = isMouseOverReset ? GRAY : LIGHTGRAY;
    
    DrawRectangleV(popupPos, popupSize, DARKGRAY);
    DrawRectangleV(resetPos, buttonSize, resetColor);
    DrawRectangleV(quitPos, buttonSize, quitColor);
    
    DrawText(resetTxt, resetPos.x + buttonSize.x / 2 - resetTxtSize / 2,
        resetPos.y + buttonSize.y / 2 - fontSize / 2, fontSize, BLACK);
    
    DrawText(quitTxt, quitPos.x + buttonSize.x / 2 - quitTxtSize / 2,
        quitPos.y + buttonSize.y / 2 - fontSize / 2, fontSize, BLACK);
    
    return 0;
}

void drawPlayerLives(Player const* p)
{
    int const fontSize = 30;
    int const gap = 10;
    if(p->numLives < 0)//if the player is dead
    {
        DrawText("YOU DIED", gap, WINDOW_HEIGHT - fontSize, fontSize, RAYWHITE);
    }
    else//if the player is still alive
    {
        char const* numPlayerLivesString = TextFormat("%d", p->numLives);
        DrawText(numPlayerLivesString, gap, WINDOW_HEIGHT - fontSize, fontSize, RAYWHITE);
    }
}

//Returns the result of drawEndPopup();
static int drawEverything(const Player* player, Texture2D* background, bool isGameOver)
{
    BeginDrawing();
    ClearBackground(BLACK);
    
    DrawTexture(*background, 0, 0, WHITE);
    drawTurrets();
    drawInvaders();
    drawBullets();
    drawPlayer(player);
    playAllActiveExplosionAnimations(GetFrameTime());
    drawPlayerLives(player);

    int endPopupResult = 0;
    if(isGameOver) { endPopupResult = drawEndPopup(); }
    
    EndDrawing();
    
    return endPopupResult;
}