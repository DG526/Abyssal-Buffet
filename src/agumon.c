
#include "simple_logger.h"
#include "agumon.h"


void agumon_update(Entity *self);

void agumon_think(Entity *self);

Entity *agumon_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no agumon for you!");
        return NULL;
    }
    ent->selectedColor = gfc_color(0.1,1,0.1,1);
    ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/dino.model");
    ent->think = agumon_think;
    ent->update = agumon_update;
    vector3d_copy(ent->position,position);
    srand(rand());
    ent->randomSeed = rand() % 3000;
    return ent;
}

void agumon_update(Entity *self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
    self->rotation.z += 0.01;
}

void agumon_think(Entity *self)
{
    if (!self)return;
    if(self->randomSeed + SDL_GetTicks() % 3000 < 1000)
        self->rotation.x += -0.002;
    else if(self->randomSeed + SDL_GetTicks() % 3000 < 2000)
        self->rotation.y += -0.002;
    else
        self->rotation.z += -0.002;
}

/*eol@eof*/
