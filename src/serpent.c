
#include "simple_logger.h"
#include "serpent.h"
#include "gfc_input.h"


void serpent_think(Entity* self);
void serpent_head_think(Entity* self);
void serpent_lure_think(Entity* self);
void serpent_segment_think(Entity* self);
void serpent_add_segment(Entity* serpent);
static int pos = 1, neg = -1;
//static int i_jawRotMult = 0;

Entity* serpent_new(Vector3D position)
{
    Entity* serpentController = NULL;

    serpentController = entity_new();
    if (!serpentController)
    {
        slog("UGH OHHHH, no serpent for you!");
        return NULL;
    }
    serpentController->childCount = 4;
    Entity* serpentTJaw = NULL; Entity* serpentBJaw = NULL; Entity* serpentLure = NULL; Entity* lastSeg = NULL;
    serpentTJaw = entity_new();
    serpentBJaw = entity_new();
    serpentLure = entity_new();
    lastSeg = entity_new();
    if (!serpentTJaw || !serpentBJaw || !serpentLure || !lastSeg)
    {
        slog("UGH OHHHH, no serpent for you!");
        return NULL;
    }

    serpentTJaw->model = gf3d_model_load("serpent_tjaw");
    serpentBJaw->model = gf3d_model_load("serpent_bjaw");
    serpentLure->model = gf3d_model_load("serpent_lure");
    lastSeg->model = gf3d_model_load("serpent_spikeseg");

    serpentTJaw->parent = serpentController;
    serpentBJaw->parent = serpentController;
    serpentLure->parent = serpentController;
    lastSeg->parent = serpentController;

    serpentTJaw->rotSpeed = neg;
    serpentBJaw->rotSpeed = pos;
    serpentTJaw->customFloat = 0;
    serpentBJaw->customFloat = 0;
    serpentLure->customFloat = 0;
    serpentTJaw->think = serpent_head_think;
    serpentBJaw->think = serpent_head_think;
    serpentLure->think = serpent_lure_think;
    lastSeg->think = serpent_segment_think;

    serpentController->children[0] = serpentTJaw;
    serpentController->children[1] = serpentBJaw;
    serpentController->children[2] = serpentLure;
    serpentController->children[3] = lastSeg;

    serpentController->think = serpent_think;
    vector3d_copy(serpentController->position, position);
    serpentController->lastTickTime = SDL_GetTicks();
    serpentController->scale = vector3d(10, 10, 10);
    serpentController->rotation.x += 90 * GFC_DEGTORAD;

    vector3d_copy(serpentTJaw->position, serpentController->position);
    vector3d_copy(serpentBJaw->position, serpentController->position);
    vector3d_copy(serpentLure->position, serpentController->position);
    vector3d_copy(lastSeg->position, serpentController->position);

    lastSeg->localScale = vector3d(0.85, 0.85, 0.85);
    lastSeg->position.y += serpentController->scale.y;
    lastSeg->followDist = serpentController->scale.y;

    for (int i = 0; i < 4; i++) {
        serpentController->children[i]->rotation.x = serpentController->rotation.x;
        vector3d_copy(serpentController->children[i]->scale, serpentController->scale);
    }

    for (int i = 0; i < 5; i++) {
        Entity* newSeg = NULL;
        newSeg = entity_new();
        if (!newSeg)
        {
            slog("UGH OHHHH, no serpent for you!");
            return NULL;
        }
        newSeg->model = gf3d_model_load("serpent_spikeseg");
        vector3d_copy(newSeg->position, serpentController->position);
        vector3d_copy(newSeg->scale, serpentController->scale);
        newSeg->localScale = vector3d(0.85 - 0.05 * i, 0.85 - 0.05 * i, 0.85 - 0.05 * i);
        lastSeg->childCount++;
        lastSeg->children[lastSeg->childCount - 1] = newSeg;
        newSeg->parent = lastSeg;
        newSeg->position.y = lastSeg->position.y + lastSeg->scale.y;
        newSeg->followDist = lastSeg->scale.y;
        newSeg->rotation.x += 90 * GFC_DEGTORAD;
        newSeg->think = serpent_segment_think;

        lastSeg = newSeg;
    }

    for (int i = 0; i < 8; i++) {
        serpent_add_segment(serpentController);
    }

    return serpentController;
}


void serpent_think(Entity* self)
{
    if (!self)return;
    //self->rotation.z += .02;
    self->lastTickTime = SDL_GetTicks();
    int moving = 0;
    if (gfc_input_key_held("d")) {
        self->rotation.z -= 0.25 * GFC_DEGTORAD;
        moving += 1;
    }
    if (gfc_input_key_held("a")) {
        self->rotation.z += 0.25 * GFC_DEGTORAD;
        moving -= 1;
    }
    if (gfc_input_key_held("s")) {
        self->rotation.x += 0.25 * GFC_DEGTORAD;
        moving += 1;
    }
    if (gfc_input_key_held("w")) {
        self->rotation.x -= 0.25 * GFC_DEGTORAD;
        moving -= 1;
    }
    if (moving != 0) {
        float dx = 0;
        float dy = 0;
        float dz = 0;

        dx = sinf(self->rotation.z);
        dy = -cosf(self->rotation.z);
        dz = sinf(self->rotation.x + 90 * GFC_DEGTORAD);

        self->position.x += dx * 0.15;
        self->position.y += dy * 0.15;
        self->position.z += dz * 0.15;
    }
}
void serpent_head_think(Entity* self) {
    if (!self || !self->parent)return;
    if (gfc_input_key_held(" ")) {
        self->customFloat += 1 * GFC_DEGTORAD;
        self->customFloat = MIN(self->customFloat, 50 * GFC_DEGTORAD);
    }
    else if (self->customFloat != 0) {
        self->customFloat -= 1 * GFC_DEGTORAD;
        self->customFloat = MAX(self->customFloat, 0);
    }

    self->position.x = self->parent->position.x;
    self->position.y = self->parent->position.y;
    self->position.z = self->parent->position.z;
    self->rotation.x = self->parent->rotation.x + self->customFloat * (self->rotSpeed > 0 ? 1 : -1);
    self->rotation.z = self->parent->rotation.z;
}
void serpent_lure_think(Entity* self) {
    if (!self || !self->parent)return;
    if (gfc_input_key_held(" ")) {
        self->customFloat += 0.05 * self->scale.y;
        self->customFloat = MIN(self->customFloat, 2 * self->scale.y);
    }
    else if (self->customFloat != 0) {
        self->customFloat -= 0.05 * self->scale.y;
        self->customFloat = MAX(self->customFloat, 0);
    }

    self->position.x = self->parent->position.x;
    self->position.y = self->parent->position.y - self->customFloat;
    self->position.z = self->parent->position.z;
    self->rotation.x = self->parent->rotation.x;
    self->rotation.z = self->parent->rotation.z;
}
void serpent_segment_think(Entity* self) {
    Vector3D diff = vector3d(self->position.x - self->parent->position.x,self->position.y - self->parent->position.y,self->position.z - self->parent->position.z);
    vector3d_normalize(&diff);
    self->rotation.x = asinf(-diff.z);
    self->rotation.z = asinf(-diff.y);
    
    self->position.x = self->parent->position.x + diff.x * self->scale.x;
    self->position.y = self->parent->position.y + diff.y * self->scale.x;
    self->position.z = self->parent->position.z + diff.z * self->scale.x;
}

void serpent_add_segment(Entity* serpent) {
    if (!serpent)return;
    Entity* lastSeg = NULL;
    int foundFinal = 0;
    lastSeg = serpent->children[serpent->childCount - 1];
    while (!foundFinal) {
        if (lastSeg->childCount == 0) {
            foundFinal = 1;
            break;
        }
        lastSeg = lastSeg->children[lastSeg->childCount - 1];
    }

    Entity* newSeg = NULL;
    newSeg = entity_new();
    if (!newSeg)
    {
        slog("UGH OHHHH, no serpent for you!");
        return NULL;
    }
    newSeg->model = gf3d_model_load("serpent_seg");
    vector3d_copy(newSeg->position, lastSeg->position);
    vector3d_copy(newSeg->scale, lastSeg->scale);
    newSeg->localScale = vector3d(0.6, 0.6, 0.6);
    lastSeg->childCount++;
    lastSeg->children[lastSeg->childCount - 1] = newSeg;
    newSeg->parent = lastSeg;
    newSeg->position.y = lastSeg->position.y + lastSeg->scale.y;
    newSeg->followDist = lastSeg->scale.y;
    newSeg->rotation.x += 90 * GFC_DEGTORAD;
    newSeg->think = serpent_segment_think;
}

/*eol@eof*/