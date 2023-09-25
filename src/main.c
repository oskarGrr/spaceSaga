#include <time.h>//time()
#include <stdlib.h>//srand()
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
static void drawEverything(const Player* player, Texture2D* background, bool endPopupIsOpen);
static bool detectCollisionsHelper(void);//Returns true if all the invaders have been eliminated.

//Helper function to reduce drawEverything()'s size.
static void drawEndPopup(void);

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
    
    bool endPopupIsOpen = true;
    while(!WindowShouldClose())
    {
        if(!endPopupIsOpen)
        {
            float dt = GetFrameTime();
            handleInput(&player, dt);
            updateBulletPositions(dt);
            updateInvaders(dt);
            updateTurrets(&player.position, dt);
        }
        
        //If all the invaders have been elmininated.
        if(detectCollisionsHelper())
        {
            endPopupIsOpen = true;
        }
        
        drawEverything(&player, &mainBackground, endPopupIsOpen);
    }
    
    UnloadTexture(player.texture);
    UnloadTexture(mainBackground);
    unloadBulletTextures();
    unloadInvaderTextures();
    unloadTurretTextures();
    releaseExplosionResources();
}

//Helper func to reduce gameLoop()'s size.
//Returns true if all the invaders have been eliminated
static bool detectCollisionsHelper(void)
{
    CollisionInfo collisionInfo;
    bool areInvadersDead = false;
    if(detectCollisions(&collisionInfo))
    {
        if(collisionInfo.typeOfEntityThatWasHit == PLAYER_WAS_HIT) { }
            //playerCollisionResolution();
        else if(collisionInfo.typeOfEntityThatWasHit == INVADER_WAS_HIT)
            areInvadersDead = invaderCollisionResolution(collisionInfo.idxOfEntityThatWasHit);
        else//If a turret was hit.
            turretCollisionResolution(collisionInfo.idxOfEntityThatWasHit);
        
        bulletCollisionResolution(collisionInfo.idxOfBullet);
    }
    
    return areInvadersDead;
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

//Helper func to reduce gameLoop()'s size.
static void drawEverything(const Player* player, Texture2D* background, bool endPopupIsOpen)
{
    BeginDrawing();
    ClearBackground(BLACK);
    
    DrawTexture(*background, 0, 0, WHITE);
    drawTurrets();
    drawInvaders();
    drawBullets();
    drawPlayer(player);
    playAllActiveExplosionAnimations(GetFrameTime());
    
    if(endPopupIsOpen)
        drawEndPopup();
    
    EndDrawing();
}

//Helper function to reduce drawEverything()'s size.
static void drawEndPopup(void)
{
    Rectangle popup =
    {
        .width = WINDOW_WIDTH * 0.25f, 
        .height = WINDOW_HEIGHT * 0.18f
    };
    
    popup.y = WINDOW_HEIGHT * 0.5f - popup.height * 0.5f;
    popup.x = WINDOW_WIDTH * 0.5f - popup.width * 0.5f;
    
    //Draw the popup background rectangle.
    DrawRectangle(popup.x, popup.y, popup.width, popup.height, DARKGRAY);
    
    const Rectangle quitButton =
    {
        .x = popup.x + popup.width * 0.1f,
        .y = popup.y + popup.height * 0.11f,
        .width = popup.width * 0.8f,
        .height = popup.height * 0.33f
    };
    
    Rectangle restartButton = quitButton;
    restartButton.y += quitButton.height + popup.height * 0.11f;
    
    const Color quitColor = isMouseOverRect(&quitButton) ? LIGHTGRAY : GRAY;
    const Color restartColor = isMouseOverRect(&restartButton) ? LIGHTGRAY : GRAY;
    
    const int fontSize = 18;
    const int halfTextHeight = GetFontDefault().baseSize / 2;
    int halfTextWidth = MeasureText("Quit", fontSize) / 2;
    
    //Draw the quit button.
    DrawRectangle(quitButton.x, quitButton.y, quitButton.width,
        quitButton.height, quitColor);
    DrawText("Quit", quitButton.x + quitButton.width * 0.5f - halfTextWidth,
        quitButton.y + quitButton.height * 0.5f - halfTextHeight, fontSize, WHITE);
    
    //Draw the restart button.
    DrawRectangle(restartButton.x, restartButton.y, restartButton.width, 
        restartButton.height, restartColor);
    halfTextWidth = MeasureText("Restart", fontSize) * 0.5f;
    DrawText("Restart", restartButton.x + restartButton.width * 0.5f - halfTextWidth, 
       restartButton.y + restartButton.height * 0.5f - halfTextHeight, fontSize, WHITE);
}