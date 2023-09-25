#ifndef EXPLOSION_H
#define EXPLOSION_H

typedef struct Vector2 Vector2;

void initExplosionsResources(void);
void releaseExplosionResources(void);

//Queues a new explosion animation at pos. pos will be the center of 
//where the explosion happens. After calling this to start a new explosion,
//call playAllActiveExplosions() to progress all current explosions by 1 frame.
void queueNewExplosionAnimation(const Vector2* pos);

//Should be called every frame inside of drawEverything() to progress  
//all of the currently active explosion animations by 1 frame.
void playAllActiveExplosionAnimations(float dt);

#endif