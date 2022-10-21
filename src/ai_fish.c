
#include "simple_logger.h"
#include "ai_fish.h"
#include "gfc_input.h"

typedef enum {
	IDLE = 0,
	ATTRACTED,
	ALERTED,
	DEAD
}FishStatus;

typedef struct {
	FishStatus status;
	float nutrition;
	float size;
	float normSpeed;
	float fullSpeed;
	void* additionalData;
}PreyData;

void oddfish_think(Entity* self);

Entity* prey_new(Vector3D position, float sizeMin, float sizeMax) {
	Entity* fish = NULL;
	float size = ((float)rand() / (float)RAND_MAX) * (sizeMax - sizeMin) + sizeMin;
	fish = oddfish_new(position, size);
	return fish;
}

Entity* oddfish_new(Vector3D position, float size) {
	Entity* fish = NULL;
	fish = entity_new();

	PreyData* pd = (PreyData*)gfc_allocate_array(sizeof(PreyData), 1);

	if (!fish || !pd) {
		slog("Whoops, not enough room for an oddfish!");
		return NULL;
	}

	pd->status = IDLE;
	pd->size = size;
	pd->nutrition = 10 * size;
	pd->normSpeed = 3 * size;
	pd->fullSpeed = 5 * pd->normSpeed;

	fish->model = gf3d_model_load("oddfish");
	fish->think = oddfish_think;
	fish->customData = pd;
	vector3d_copy(fish->position, position);
	float realSize = size;
	fish->scale = vector3d(realSize, realSize, realSize);
	float rotZ = ((float)rand() / (float)RAND_MAX) * GFC_2PI;
	fish->rotation = vector3d(0, 0, rotZ);
	
	return fish;
}
void oddfish_think(Entity* self) {

}