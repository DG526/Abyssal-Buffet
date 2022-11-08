
#include "simple_logger.h"
#include "ai_fish.h"
#include "gfc_input.h"
#include "serpent.h"

void prey_think_standard(Entity* self);
void checkForDespawn(Entity* self);

Entity* spawn_new_around_serpent(Entity* serpent, float radius, int ticksElapsed) {
	float yaw = ((float)rand() / (float)RAND_MAX) * GFC_2PI;
	float pitch = ((float)rand() / (float)RAND_MAX) * GFC_PI - GFC_HALF_PI;
	Vector3D spawnPos = serpent->position;
	float dx = sinf(yaw);
	float dy = -cosf(yaw);
	float dz = sinf(pitch);
	spawnPos.x -= dx * radius;
	spawnPos.y -= dy * radius;
	spawnPos.z += dz * radius;

	float difficultySeed = SDL_clamp(((SerpentData*)(serpent->customData))->size * (ticksElapsed / 120000), 1, 20);
	float sizemin, sizemax, predatorChance;

	if (difficultySeed < 10) {
		sizemin = difficultySeed * 0.5f;
		sizemax = difficultySeed * 1.2f;
	}
	else {

		sizemin = difficultySeed * 0.7f;
		sizemax = difficultySeed * 1.5f;
	}
	return prey_new(spawnPos, sizemin, sizemax, serpent);
}

Entity* prey_new(Vector3D position, float sizeMin, float sizeMax, Entity* serpent) {
	Entity* fish = NULL;
	float size = ((float)rand() / (float)RAND_MAX) * (sizeMax - sizeMin) + sizeMin;

	float seed = ((float)rand() / (float)RAND_MAX) * 3.5;
	if (seed < 1)
		fish = oddfish_new(position, size, serpent);
	else if (seed < 2)
		fish = hogfish_new(position, size, serpent);
	else if (seed < 3)
		fish = mossprawn_new(position, size, serpent);
	else
		fish = albeyeno_new(position, size, serpent);
	fish->entityType = ET_PREY;
	fish->onFear = prey_flee;
	return fish;
}

Entity* oddfish_new(Vector3D position, float size, Entity* serpent) {
	size = min(size, 3.5f);
	Entity* fish = NULL;
	fish = fish_new();

	PreyData* pd = (PreyData*)gfc_allocate_array(sizeof(PreyData), 1);

	if (!fish || !pd) {
		slog("Whoops, not enough room for an oddfish!");
		return NULL;
	}

	float rotZ = ((float)rand() / (float)RAND_MAX) * GFC_2PI;
	fish->rotation = vector3d(0, 0, rotZ);

	pd->status = IDLE;
	pd->serpent = serpent;
	pd->serpentLure = serpent->children[2];
	pd->size = size;
	pd->nutrition = 10 * size;
	pd->normSpeed = 3 * size;
	pd->fullSpeed = 5 * pd->normSpeed;
	for (int i = 0; i < 5; i++) {
		pd->reward[i] = 0;
	}
	pd->reward[0] = (int)floorf(4 * size);
	vector3d_copy(pd->targetDirection, fish->rotation);
	vector3d_copy(pd->lastDirection, fish->rotation);

	pd->snappable = 0;

	fish->model = gf3d_model_load("models/oddfish.model");
	fish->think = prey_think_standard;
	fish->customData = pd;
	vector3d_copy(fish->position, position);
	float realSize = size * 0.45f;
	fish->scale = vector3d(realSize, realSize, realSize);

	fish->bounds = gfc_sphere(fish->position.x, fish->position.y, fish->position.z, 3.5f * realSize);
	fish->alertBounds = gfc_sphere(fish->position.x, fish->position.y, fish->position.z, 4.5f * realSize);
	
	return fish;
}
Entity* hogfish_new(Vector3D position, float size, Entity* serpent) {
	size = min(size, 5);
	Entity* fish = NULL;
	fish = fish_new();

	PreyData* pd = (PreyData*)gfc_allocate_array(sizeof(PreyData), 1);

	if (!fish || !pd) {
		slog("Whoops, not enough room for an hogfish!");
		return NULL;
	}

	float rotZ = ((float)rand() / (float)RAND_MAX) * GFC_2PI;
	fish->rotation = vector3d(0, 0, rotZ);

	pd->status = IDLE;
	pd->serpent = serpent;
	pd->serpentLure = serpent->children[2];
	pd->size = size;
	pd->nutrition = 13 * size;
	pd->normSpeed = 2 * size;
	pd->fullSpeed = 9 * pd->normSpeed;
	for (int i = 0; i < 5; i++) {
		pd->reward[i] = 0;
	}
	pd->reward[0] = (int)floorf(3 * size);
	float hasGoldite = ((float)rand() / (float)RAND_MAX) * 2 - 1;
	if (hasGoldite > 0) {
		pd->reward[1] = (int)floorf(size / 2);
	}
	else {
		pd->reward[2] = (int)floorf(size / 2);
	}
	vector3d_copy(pd->targetDirection, fish->rotation);
	vector3d_copy(pd->lastDirection, fish->rotation);

	pd->snappable = 0;

	fish->model = gf3d_model_load("models/hogfish.model");
	fish->think = prey_think_standard;
	fish->customData = pd;
	vector3d_copy(fish->position, position);
	float realSize = size * 0.45f;
	fish->scale = vector3d(realSize, realSize, realSize);

	fish->bounds = gfc_sphere(fish->position.x, fish->position.y, fish->position.z, 3.5f * realSize);
	fish->alertBounds = gfc_sphere(fish->position.x, fish->position.y, fish->position.z, 4.5f * realSize);
	
	return fish;
}
Entity* mossprawn_new(Vector3D position, float size, Entity* serpent) {
	size = min(size, 3);
	Entity* fish = NULL;
	fish = fish_new();

	PreyData* pd = (PreyData*)gfc_allocate_array(sizeof(PreyData), 1);

	if (!fish || !pd) {
		slog("Whoops, not enough room for an mossprawn!");
		return NULL;
	}

	float rotZ = ((float)rand() / (float)RAND_MAX) * GFC_2PI;
	fish->rotation = vector3d(0, 0, rotZ);

	pd->status = IDLE;
	pd->serpent = serpent;
	pd->serpentLure = serpent->children[2];
	pd->size = size;
	pd->nutrition = 10 * size;
	pd->normSpeed = 2 * size;
	pd->fullSpeed = 4 * pd->normSpeed;
	for (int i = 0; i < 5; i++) {
		pd->reward[i] = 0;
	}
	pd->reward[0] = (int)floorf(2 * size);
	float hasGoldite = ((float)rand() / (float)RAND_MAX) * 2 - 1;
	if (hasGoldite > 0) {
		pd->reward[1] = (int)floorf(size * 4 / 5);
	}
	else {
		pd->reward[2] = (int)floorf(size * 4 / 5);
	}
	vector3d_copy(pd->targetDirection, fish->rotation);
	vector3d_copy(pd->lastDirection, fish->rotation);

	pd->snappable = 0;

	fish->model = gf3d_model_load("models/mossprawn.model");
	fish->think = prey_think_standard;
	fish->customData = pd;
	vector3d_copy(fish->position, position);
	float realSize = size * 0.45f;
	fish->scale = vector3d(realSize, realSize, realSize);

	fish->bounds = gfc_sphere(fish->position.x, fish->position.y, fish->position.z, 3.5f * realSize);
	fish->alertBounds = gfc_sphere(fish->position.x, fish->position.y, fish->position.z, 4.5f * realSize);
	
	return fish;
}
Entity* albeyeno_new(Vector3D position, float size, Entity* serpent) {
	size = min(size, 7);
	Entity* fish = NULL;
	fish = fish_new();

	PreyData* pd = (PreyData*)gfc_allocate_array(sizeof(PreyData), 1);

	if (!fish || !pd) {
		slog("Whoops, not enough room for an albeyeno!");
		return NULL;
	}

	float rotZ = ((float)rand() / (float)RAND_MAX) * GFC_2PI;
	fish->rotation = vector3d(0, 0, rotZ);

	pd->status = IDLE;
	pd->serpent = serpent;
	pd->serpentLure = serpent->children[2];
	pd->size = size;
	pd->nutrition = 10 * size;
	pd->normSpeed = 4 * size;
	pd->fullSpeed = 2 * pd->normSpeed;
	for (int i = 0; i < 5; i++) {
		pd->reward[i] = 0;
	}
	pd->reward[0] = (int)floorf(5 * size);
	float hasGoldite = ((float)rand() / (float)RAND_MAX) * 2 - 1;
	if (hasGoldite > 0) {
		pd->reward[1] = (int)floorf(size);
	}
	else {
		pd->reward[2] = (int)floorf(size);
	}
	vector3d_copy(pd->targetDirection, fish->rotation);
	vector3d_copy(pd->lastDirection, fish->rotation);

	pd->snappable = 0;

	fish->model = gf3d_model_load("models/albeyeno.model");
	fish->think = prey_think_standard;
	fish->customData = pd;
	vector3d_copy(fish->position, position);
	float realSize = size * 0.45f;
	fish->scale = vector3d(realSize, realSize, realSize);

	fish->bounds = gfc_sphere(fish->position.x, fish->position.y, fish->position.z, 3.5f * realSize);
	fish->alertBounds = gfc_sphere(fish->position.x, fish->position.y, fish->position.z, 4.5f * realSize);
	
	return fish;
}
void prey_think_standard(Entity* self) {
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
		/*
		float dx = 0;
		float dy = 0;
		float dz = 0;

		dx = sinf(self->rotation.z);
		dy = -cosf(self->rotation.z);
		dz = sinf(self->rotation.x);
		*/

		Vector3D d = vector3d(0, 0, 0);
		//vector3d_angle_vectors(vector3d(self->rotation.x * GFC_RADTODEG,self->rotation.y * GFC_RADTODEG,self->rotation.z * GFC_RADTODEG), &d, NULL, NULL);

		float dx = 0;
		float dy = 0;
		float dz = 0;

		dx = sinf(self->rotation.z);
		dy = -cosf(self->rotation.z);
		dz = sinf(self->rotation.x);

		self->position.x += dx * 0.015 * self->scale.y * pd->normSpeed;
		self->position.y += dy * 0.015 * self->scale.y * pd->normSpeed;
		self->position.z += dz * 0.015 * self->scale.y * pd->normSpeed;
		vector3d_copy(self->bounds, self->position);

		if (((SerpentData*)(pd->serpent->customData))->luring && gfc_sphere_overlap(self->bounds, pd->serpentLure->fearBounds)) {
			pd->status = ATTRACTED;
			pd->lastDirection = self->rotation;

			Vector3D diff = vector3d(self->position.x - pd->serpentLure->position.x, self->position.y - pd->serpentLure->position.y, self->position.z - pd->serpentLure->position.z);

			//slog("Fish is swimming towards %f, %f, %f.", pd->serpentLure->position.x, pd->serpentLure->position.y, pd->serpentLure->position.z);

			//Vector3D diff = vector3d(pd->serpentLure->position.x - self->position.x, pd->serpentLure->position.y - self->position.y, pd->serpentLure->position.z - self->position.z);

			vector3d_normalize(&diff);
			//vector3d_angles(diff, &pd->targetDirection);

			pd->targetDirection = vector3d(vector_angle(-diff.z, diff.x + diff.y) * GFC_DEGTORAD, 0, vector_angle(diff.x, diff.y) * GFC_DEGTORAD);
			self->rotation = pd->targetDirection;
			//slog("A fish is attracted.");
			slog_sync();
		}
		break;
	case ATTRACTED:{
		if(!pd->snappable){
			if (!((SerpentData*)(pd->serpent->customData))->luring) {
				pd->status = IDLE;
				return;
			}

			Vector3D diff = vector3d(self->position.x - pd->serpentLure->position.x, self->position.y - pd->serpentLure->position.y, self->position.z - pd->serpentLure->position.z);

			//slog("Fish is swimming towards %f, %f, %f.", pd->serpentLure->position.x, pd->serpentLure->position.y, pd->serpentLure->position.z);

			//Vector3D diff = vector3d(pd->serpentLure->position.x - self->position.x, pd->serpentLure->position.y - self->position.y, pd->serpentLure->position.z - self->position.z);

			vector3d_normalize(&diff);
			//vector3d_angles(diff, &pd->targetDirection);

			pd->targetDirection = vector3d(vector_angle(-diff.z, diff.x + diff.y) * GFC_DEGTORAD, 0, vector_angle(diff.x, diff.y) * GFC_DEGTORAD);
			self->rotation = pd->targetDirection;

			Vector3D d = vector3d(0,0,0);
			//vector3d_angle_vectors(vector3d(self->rotation.x * GFC_RADTODEG,self->rotation.y * GFC_RADTODEG,self->rotation.z * GFC_RADTODEG), &d, NULL, NULL);

			float dx = 0;
			float dy = 0;
			float dz = 0;

			dx = sinf(self->rotation.z);
			dy = -cosf(self->rotation.z);
			dz = sinf(self->rotation.x);

			self->position.x -= dx * 0.015 * self->scale.y * pd->normSpeed;
			self->position.y -= dy * 0.015 * self->scale.y * pd->normSpeed;
			self->position.z += dz * 0.015 * self->scale.y * pd->normSpeed;
			vector3d_copy(self->bounds, self->position);

			//slog("Fish is swimming at %f, %f, %f.", self->position.x, self->position.y, self->position.z);

			if (gfc_sphere_overlap(self->bounds, pd->serpentLure->bounds)) {
				gfc_list_append(((SerpentData*)(pd->serpent->customData))->snappables, self);
				pd->snappable = 1;
			}
		}
		break;
	}
	case ALERTED: {
		pd->fearTimer -= 1;
		float dx = 0;
		float dy = 0;
		float dz = 0;

		dx = sinf(self->rotation.z);
		dy = -cosf(self->rotation.z);
		dz = sinf(self->rotation.x);

		self->position.x += dx * 0.015 * self->scale.y * pd->fullSpeed;
		self->position.y += dy * 0.015 * self->scale.y * pd->fullSpeed;
		self->position.z += dz * 0.015 * self->scale.y * pd->fullSpeed;
		if (pd->fearTimer <= 0) {
			pd->status = IDLE;
		}
		break;
	}
	default:
		break;
	}
	checkForDespawn(self);
}

void prey_flee(Entity* self, Entity* scarer) {
	if (!self) return;
	PreyData* pd = ((PreyData*)(self->customData));
	pd->status = ALERTED;
	pd->lastDirection = self->rotation;

	//Vector3D diff = vector3d(self->position.x - pd->serpentLure->position.x, self->position.y - pd->serpentLure->position.y, self->position.z - pd->serpentLure->position.z);

	//slog("Fish is swimming towards %f, %f, %f.", pd->serpentLure->position.x, pd->serpentLure->position.y, pd->serpentLure->position.z);

	Vector3D diff = vector3d(scarer->position.x - self->position.x, scarer->position.y - self->position.y, scarer->position.z - self->position.z);

	vector3d_normalize(&diff);
	//vector3d_angles(diff, &pd->targetDirection);

	pd->targetDirection = vector3d(vector_angle(-diff.z, diff.x + diff.y) * GFC_DEGTORAD, 0, vector_angle(diff.x, diff.y) * GFC_DEGTORAD);
	self->rotation = pd->targetDirection;

	pd->fearTimer = 4000;
}

void onReachedLure(Entity* self) {
	if (!self) return;
	((PreyData*)(self->customData))->snappable = true;
	gfc_list_append(&((SerpentData*)(((PreyData*)(self->customData))->serpent->customData))->snappables, self);
}

void checkForDespawn(Entity* self) {
	if (!self)
		return;
	switch (self->entityType) {
	case ET_PREY: {
		PreyData* pd = ((PreyData*)(self->customData));
		float difference = vector3d_magnitude_between(self->position, pd->serpent->position);

		if (difference > 70) {
			fish_free(self, 0);
		}

		break;
	}
	case ET_SWITCH:
	case ET_PREDATOR: {
		PreyData* pd = ((PreyData*)(self->customData));
		float difference = vector3d_magnitude_between(self->position, pd->serpent->position);

		if (difference > 90) {
			fish_free(self, 1);
		}

		break;
	}
	}
}