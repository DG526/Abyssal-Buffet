
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
	Vector3D targetDirection;
	Vector3D lastDirection;
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

	float rotZ = ((float)rand() / (float)RAND_MAX) * GFC_2PI;
	fish->rotation = vector3d(0, 0, rotZ);

	pd->status = IDLE;
	pd->size = size;
	pd->nutrition = 10 * size;
	pd->normSpeed = 3 * size;
	pd->fullSpeed = 5 * pd->normSpeed;
	vector3d_copy(pd->targetDirection, fish->rotation);
	vector3d_copy(pd->lastDirection, fish->rotation);

	fish->model = gf3d_model_load("oddfish");
	fish->think = oddfish_think;
	fish->customData = pd;
	vector3d_copy(fish->position, position);
	float realSize = size;
	fish->scale = vector3d(realSize, realSize, realSize);
	
	return fish;
}
void oddfish_think(Entity* self) {
	if (!self) return;
	float maxXR = 15.0f * GFC_DEGTORAD;
	float minXR = -maxXR;
	PreyData* pd = ((PreyData*)(self->customData));
	switch (pd->status)
	{
	case IDLE:
		if (vector3d_equal(pd->targetDirection, self->rotation)) {
			pd->lastDirection = self->rotation;
			float randX = ((float)rand() / (float)RAND_MAX) * (maxXR - minXR) + minXR;
			float randZ = ((float)rand() / (float)RAND_MAX) * GFC_2PI;
			pd->targetDirection = vector3d(randX, 0, randZ);
		}
		Vector3D angleDiff = vector3d(pd->targetDirection.x - pd->lastDirection.x, 0, pd->targetDirection.z - pd->lastDirection.z);
		self->rotation.x += angleDiff.x / 1500;
		self->rotation.z += angleDiff.z / 1500;
		if (fabs(pd->targetDirection.x - self->rotation.x) < 2.5f * GFC_DEGTORAD) {
			self->rotation.x = pd->targetDirection.x;
		}
		if (fabs(pd->targetDirection.z - self->rotation.z) < 2.5f * GFC_DEGTORAD) {
			self->rotation.z = pd->targetDirection.z;
		}
		float dx = 0;
		float dy = 0;
		float dz = 0;

		dx = sinf(self->rotation.z);
		dy = -cosf(self->rotation.z);
		dz = sinf(self->rotation.x);

		self->position.x -= dx * 0.015 * self->scale.y * ((PreyData*)(self->customData))->normSpeed;
		self->position.y -= dy * 0.015 * self->scale.y * ((PreyData*)(self->customData))->normSpeed;
		self->position.z += dz * 0.015 * self->scale.y * ((PreyData*)(self->customData))->normSpeed;
		break;
	default:
		break;
	}
}