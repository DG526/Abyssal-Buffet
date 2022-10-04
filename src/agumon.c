
#include "simple_logger.h"
#include "agumon.h"


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
    
    ent->model = gf3d_model_load("dino");
    ent->think = agumon_think;
    vector3d_copy(ent->position,position);
    srand(rand());
    ent->randomSeed = rand() % 3000;
    return ent;
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
