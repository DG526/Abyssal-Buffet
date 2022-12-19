
#include "simple_logger.h"
#include "ai_fish.h"
#include "gfc_input.h"
#include "serpent.h"

void scythefish_think(Entity* self);

Entity* spawn_scythefish(Entity* serpent) {
	Entity* fish = NULL;
	fish = entity_new();
	ScythefishData* sfd = (ScythefishData*)gfc_allocate_array(sizeof(ScythefishData), 1);
	if (!fish || !sfd) {
		slog("Whoops, not enough room for the scythefish. You got lucky....");
		return NULL;
	}

	sfd->serpent = serpent;
	fish->customData = sfd;
	fish->think = scythefish_think;
	fish->scale = vector3d(2.3, 2.3, 2.3);
	fish->bounds = gfc_sphere(0, 0, 0, fish->scale.x);
	fish->entityType = ET_PREDATOR;
	fish->model = gf3d_model_load("models/scythefish.model");
	if (!fish->model) slog("Oooooooo... It's a ghoooooost!!");
	fish->color = gfc_color(1, 1, 1, 1);
	fish->position = vector3d(0, 0, 0);
	slog("The scythefish has appeared!!");
	return fish;
}

void scythefish_think(Entity* self) {
	if (!self) return;
	if (gfc_input_key_pressed("q")) {
		slog("Scythefish is at (%f, %f, %f).", self->position.x, self->position.y, self->position.z);
	}
	ScythefishData* sfd = (ScythefishData*)(self->customData);
	if (!sfd || !sfd->serpent) return;

	if (vector3d_distance_between_less_than(self->position, sfd->serpent->position, 4.5)) {
		hurtSerpent(sfd->serpent, self, ((SerpentData*)(sfd->serpent->customData))->healthMax);
		slog("YOUR SOUL HAS BEEN REAPED BY THE SCYTHEFISH!!");
		return;
	}

	Vector3D diff = vector3d(sfd->serpent->position.x - self->position.x, sfd->serpent->position.y - self->position.y, sfd->serpent->position.z - self->position.z);
	vector3d_normalize(&diff);
	self->rotation = vector3d(vector_angle(-diff.z, diff.x + diff.y) * GFC_DEGTORAD, 0, vector_angle(diff.x, diff.y) * GFC_DEGTORAD);

	float dx = 0;
	float dy = 0;
	float dz = 0;

	dx = sinf(self->rotation.z + GFC_PI);
	dy = -cosf(self->rotation.z + GFC_PI);
	dz = sinf(self->rotation.x + GFC_PI);

	self->position.x -= dx * 0.015 * 5;
	self->position.y -= dy * 0.015 * 5;
	self->position.z += dz * 0.015 * 5;
}