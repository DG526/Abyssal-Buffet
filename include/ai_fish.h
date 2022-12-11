#ifndef __FISH_H__
#define __FISH_H__

#include "entity.h"

typedef enum {
	IDLE = 0,
	ATTRACTED,
	ALERTED,
	HUNGRY,
	DEAD
}FishStatus;

typedef enum {
	UNDEF = 0,
	ODDFISH,
	HOGFISH,
	MOSSPRAWN,
	ALBEYENO,
	ORCA,
	SEAHORNET,
	FREAK
}FishIdentity;

typedef enum {
	TGT_CHEMOSYNTHESIS,
	TGT_LARGEST,
	TGT_SMALLEST,
	TGT_NUTRITION,
	TGT_FIRSTSPOTTED
}HuntingStrategy;

typedef struct {
	FishIdentity identity;
	Entity* serpentLure;
	Entity* serpent;
	FishStatus status;
	int score;
	float nutrition; //Determines sustinence and growth.
	int reward[5];
	float size;
	float normSpeed;
	float fullSpeed;
	Vector3D targetDirection;
	Vector3D lastDirection;
	int snappable;
	void* additionalData;
	int fearTimer;
	int eats;
	HuntingStrategy strategy;
	Entity* target;
	float targetSize;
	float targetNutrition;
	float cannibalThreshold;
	float pursuitLimit;
	float lungeThreshold;
	int hunted;
	Entity* hunter;
	float damage;
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
Entity* prey_new_from_file(FishIdentity species, Vector3D position, float size, Entity* serpent);

Entity* oddfish_new(Vector3D position, float size, Entity* serpent);
Entity* hogfish_new(Vector3D position, float size, Entity* serpent);
Entity* mossprawn_new(Vector3D position, float size, Entity* serpent);
Entity* albeyeno_new(Vector3D position, float size, Entity* serpent);
Entity* orca_new(Vector3D position, float size, Entity* serpent);


void onReachedLure(Entity* self);
void prey_flee(Entity* self, Entity* scarer);
void chaseFish(Entity* self, Entity* target);
void chaseSerpent(Entity* self, Entity* target);
int cannibalInvalidityCheck(Entity* self, Entity* target);
int isOutOfReach(Entity* self, Entity* target);
int canLunge(Entity* self, Entity* target);

#endif