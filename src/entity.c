#include <stdlib.h>
#include <string.h>

#include "simple_logger.h"

#include "entity.h"
#include "serpent.h"

typedef struct
{
    Entity *entity_list;
    Uint32  entity_count;
    int fish_count;
    int pred_count;
    
}EntityManager;

static EntityManager entity_manager = {0};

void entity_system_close()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        entity_free(&entity_manager.entity_list[i]);        
    }
    free(entity_manager.entity_list);
    memset(&entity_manager,0,sizeof(EntityManager));
    slog("entity_system closed");
}

void entity_system_init(Uint32 maxEntities)
{
    entity_manager.entity_list = gfc_allocate_array(sizeof(Entity),maxEntities);
    if (entity_manager.entity_list == NULL)
    {
        slog("failed to allocate entity list, cannot allocate ZERO entities");
        return;
    }
    entity_manager.entity_count = maxEntities;
    atexit(entity_system_close);
    slog("entity_system initialized");
}

Entity *entity_new()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet, so we can!
        {
            entity_manager.entity_list[i]._inuse = 1;
            gfc_matrix_identity(entity_manager.entity_list[i].modelMat);
            entity_manager.entity_list[i].scale.x = 1;
            entity_manager.entity_list[i].scale.y = 1;
            entity_manager.entity_list[i].scale.z = 1;
            entity_manager.entity_list[i].localScale.x = 1;
            entity_manager.entity_list[i].localScale.y = 1;
            entity_manager.entity_list[i].localScale.z = 1;
            
            entity_manager.entity_list[i].color = gfc_color(1,1,1,1);
            entity_manager.entity_list[i].selectedColor = gfc_color(1,1,1,1);
            
            return &entity_manager.entity_list[i];
        }
    }
    slog("entity_new: no free space in the entity list");
    return NULL;
}
Entity* fish_new() {
    Entity* fish = entity_new();
    if (fish) {
        entity_manager.fish_count += 1;
    }
    return fish;
}
Entity* pred_new() {
    Entity* fish = entity_new();
    if (fish) {
        entity_manager.fish_count += 1;
        entity_manager.pred_count += 1;
    }
    return fish;
}

int get_fish_count() {
    return entity_manager.fish_count;
}
int get_predator_count() {
    return entity_manager.pred_count;
}

void entity_free(Entity *self)
{
    if (!self)return;
    //MUST DESTROY
    gf3d_model_free(self->model);
    memset(self,0,sizeof(Entity));
}

void entity_free_all() {
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_free(&entity_manager.entity_list[i]);
    }
    entity_manager.fish_count = 0;
    entity_manager.pred_count = 0;
}

void fish_free(Entity* self, int isPred) {
    if (!self)return;
    entity_manager.fish_count -= 1;
    entity_manager.pred_count -= isPred;
    entity_free(self);
}


void entity_draw(Entity *self)
{
    if (!self)return;
    if (self->hidden)return;
    gf3d_model_draw(self->model,self->modelMat,gfc_color_to_vector4f(self->color),vector4d(1,1,1,1));
    if (self->selected)
    {
        gf3d_model_draw_highlight(
            self->model,
            self->modelMat,
            gfc_color_to_vector4f(self->selectedColor));
    }
}

void entity_draw_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_draw(&entity_manager.entity_list[i]);
    }
}

void entity_think(Entity *self)
{
    if (!self)return;
    if (self->think)self->think(self);
}

void entity_think_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_think(&entity_manager.entity_list[i]);
    }
}


void entity_update(Entity *self)
{
    if (!self)return;
    // HANDLE ALL COMMON UPDATE STUFF
    
    vector3d_add(self->position,self->position,self->velocity);
    vector3d_add(self->velocity,self->acceleration,self->velocity);
    
    gfc_matrix_identity(self->modelMat);
    Vector3D truescale = vector3d(self->scale.x * self->localScale.x, self->scale.y * self->localScale.y, self->scale.z * self->localScale.z);
    gfc_matrix_scale(self->modelMat,truescale);
    /*
    gfc_matrix_rotate(self->modelMat,self->modelMat,self->rotation.z,vector3d(0,0,1));
    gfc_matrix_rotate(self->modelMat,self->modelMat,self->rotation.y,vector3d(0,1,0));
    gfc_matrix_rotate(self->modelMat,self->modelMat,self->rotation.x,vector3d(1,0,0));
    */
    
    gfc_matrix_scale(self->modelMat,self->scale);
    gfc_matrix_rotate_by_vector(self->modelMat,self->modelMat,self->rotation);
    gfc_matrix_translate(self->modelMat,self->position);
    
    if (self->update)self->update(self);
}

void entity_update_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_update(&entity_manager.entity_list[i]);
    }
}

void entity_collide_check(Entity* ent) {
    Sphere boundA, boundB;
    if (!ent) return;
    memcpy(&boundA, &ent->bounds, sizeof(Sphere));
    vector3d_add(boundA, boundA, ent->position);
    for (int i = 0; i < entity_manager.entity_count; i++) {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        memcpy(&boundB, &entity_manager.entity_list[i].bounds, sizeof(Sphere));
        vector3d_add(boundB, boundB, entity_manager.entity_list[i].position);
        if (gfc_sphere_overlap(boundA, boundB)) {
            //DO SOMETHING!!
        }
    }
}
void entity_collide_all() {
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        if (!entity_manager.entity_list[i].clips)// does not clip
        {
            continue;// skip this iteration of the loop
        }
        entity_collide_check(&entity_manager.entity_list[i]);
    }
}

void entity_fearCheck(Entity* ent) {
    Sphere alert, fright;
    if (!ent) return;
    memcpy(&alert, &ent->alertBounds, sizeof(Sphere));
    //vector3d_add(boundA, boundA, ent->position);
    for (int i = 0; i < entity_manager.entity_count; i++) {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        if (ent->entityType == ET_SWITCH && entity_manager.entity_list[i].entityType == ET_SWITCH) {
            continue;
        }
        if (entity_manager.entity_list[i].entityType == ET_PREY || entity_manager.entity_list[i].entityType == ET_DEFAULT || entity_manager.entity_list[i].entityType == ET_LURE) {
            continue;
        }
        if (entity_manager.entity_list[i].fearsomeness == 0 || entity_manager.entity_list[i].fearsomeness <= ent->fearThreshold){
            continue;
        }
        memcpy(&fright, &entity_manager.entity_list[i].fearBounds, sizeof(Sphere));
        if (entity_manager.entity_list[i].entityType == ET_SERPENTCONTROLLER) {
            if (((SerpentData*)(entity_manager.entity_list[i].customData))->still) {
                memcpy(&fright, &entity_manager.entity_list[i].bounds, sizeof(Sphere)); //use contact bounds instead.
            }
        }
        if (entity_manager.entity_list[i].entityType == ET_SERPENTPART) {
            Entity* lead = &entity_manager.entity_list[i];
            while (lead->parent) {
                lead = lead->parent;
            }
            if (((SerpentData*)(lead->customData))->still) {
                memcpy(&fright, &entity_manager.entity_list[i].bounds, sizeof(Sphere)); //use contact bounds instead.
            }
        }
        //vector3d_add(fright, fright, entity_manager.entity_list[i].position);
        if (gfc_sphere_overlap(alert, fright)) {
            ent->onFear(ent, &entity_manager.entity_list[i]);
            return;
        }
    }
}

void entity_fearCheck_all() {
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        if (entity_manager.entity_list[i].entityType == ET_DEFAULT ||
            entity_manager.entity_list[i].entityType == ET_PREDATOR ||
            entity_manager.entity_list[i].entityType == ET_SERPENTCONTROLLER ||
            entity_manager.entity_list[i].entityType == ET_SERPENTPART ||
            entity_manager.entity_list[i].entityType == ET_LURE)// not capable of fear
        {
            continue;// skip this iteration of the loop
        }
        if (entity_manager.entity_list[i].fearThreshold == -1)// does not fear
        {
            continue;// skip this iteration of the loop
        }
        entity_collide_check(&entity_manager.entity_list[i]);
    }
}

/*eol@eof*/
