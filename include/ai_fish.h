#ifndef __FISH_H__
#define __FISH_H__

#include "entity.h"

typedef enum {
	IDLE = 0,
	ATTRACTED,
	ALERTED,
	DEAD
}FishStatus;

typedef struct {
	Entity* serpentLure;
	Entity* serpent;
	FishStatus status;
	float nutrition;
	int reward[5];
	float size;
	float normSpeed;
	float fullSpeed;
	Vector3D targetDirection;
	Vector3D lastDirection;
	int snappable;
	void* additionalData;
	int fearTimer;
}PreyData;

/**
 * @brief Create a new random prey entity
 * @param position where to spawn the serpent at
 * @param sizeMin the minimum size prey to spawn
 * @param sizeMax the maximum size prey to spawn
 * @return NULL on error, or an serpent entity pointer on success
 */
Entity* spawn_new_around_serpent(Entity* serpent, float radius, int ticksElapsed);

Entity* prey_new(Vector3D position, float sizeMin, float sizeMax, Entity* serpent);

Entity* oddfish_new(Vector3D position, float size, Entity* serpent);
Entity* hogfish_new(Vector3D position, float size, Entity* serpent);
Entity* mossprawn_new(Vector3D position, float size, Entity* serpent);
Entity* albeyeno_new(Vector3D position, float size, Entity* serpent);


void onReachedLure(Entity* self);
void prey_flee(Entity* self, Entity* scarer);

#endif