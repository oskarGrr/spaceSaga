#include "raylib.h"
#include "Player.h"
#include "Utilities.h"
#include "windowSize.h"

void playerCtor(Player* this)
{
    this->texture = LoadTexture("resources/textures/playerShip.png");
    this->position = (Vector2){WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.85f};
    this->numLives = 3;
    this->velocity = 420.0f;
    this->shootCooldownTime = 0.65f;
}

void playerPositionUpdate(Player* this, LeftOrRight leftOrRight, float dt)
{
    float modulatedVelocity = this->velocity * dt;
    
    if(leftOrRight == RIGHT)
    {
        //If we haven't reached the right side of the window.
        if(this->position.x + (this->texture.width * 0.5f) < WINDOW_WIDTH)
            this->position.x += modulatedVelocity;//Then take a step to the right.
    }
    else if(leftOrRight == LEFT)
    {
        //If we haven't reached the left side of the window.
        if(this->position.x - (this->texture.width * 0.5f) > 0)
            this->position.x -= modulatedVelocity;//Then take a step to the left.
    }
}

void drawPlayer(const Player* this)
{
    Vector2 playerTexturePos;
    playerTexturePos.x = this->position.x - this->texture.width * 0.5f;
    playerTexturePos.y = this->position.y - this->texture.width * 0.5f;
    DrawTextureV(this->texture, playerTexturePos, WHITE);
}