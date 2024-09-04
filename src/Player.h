#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"
#include <stddef.h>//size_t
#include "Utilities.h"

typedef enum {LEFT, RIGHT} LeftOrRight;

#define PLAYER_NUM_HIT_CIRCLES 6

typedef struct
{
    Vector2 position;//position is in the middle of the player ship
    Texture2D texture;
    int numLives;
    float velocity;//left and right velocity
    float shootCooldownTime;//cooldown on how fast the player can shoot in seconds
    Circle hitCircles[PLAYER_NUM_HIT_CIRCLES];
} Player;

float getPlayerTopOfTextureYCoord(Player const*);
void resetPlayer(Player*);
void playerCtor(Player*);
void playerPositionUpdate(Player*, LeftOrRight, float dt);
bool playerCollisionResolution(Player*);
void drawPlayer(const Player*);

#endif