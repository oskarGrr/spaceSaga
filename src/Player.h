#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"
#include <stddef.h>//size_t

typedef enum {LEFT, RIGHT} LeftOrRight;

typedef struct
{
    Vector2 position;//position is in the middle of the player ship
    Texture2D texture;
    int numLives;
    float velocity;//left and right velocity
    float shootCooldownTime;//cooldown on how fast the player can shoot in seconds
} Player;

void playerCtor(Player*);
void playerPositionUpdate(Player*, LeftOrRight, float dt);
void drawPlayer(const Player*);

#endif