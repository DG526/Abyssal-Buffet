
#include "simple_logger.h"
#include "serpent.h"
#include "gfc_input.h"
#include "gf3d_camera.h"
#include "gf3d_vgraphics.h"
#include "ai_fish.h"


void serpent_think(Entity* self);
void serpent_update(Entity* self);
void serpent_head_think(Entity* self);
void serpent_lure_think(Entity* self);
void serpent_segment_think(Entity* self);
void serpent_add_segment(Entity* serpent);
static int pos = 1, neg = -1;
//static int i_jawRotMult = 0;



Entity* serpent_new(Vector3D position, SerpentPersStats *persStats, PersCurrencies* wallet)
{
    Entity* serpentController = NULL;

    serpentController = entity_new();

    SerpentData* sd = (SerpentData*)gfc_allocate_array(sizeof(SerpentData), 1);

    if (!serpentController || !sd)
    {
        slog("UGH OHHHH, no serpent for you!");
        return NULL;
    }

    sd->zoom = 1;

    sd->healthMax = 50;
    sd->health = 50;
    sd->hunger = 0;
    sd->hungerRate = 1.0f / 3.0f / 60.0f;
    sd->size = 1;
    sd->length = 13;
    sd->still = 1;
    sd->persStats = persStats;
    sd->currencies = wallet;
    sd->snappables = gfc_list_new();

    slog("Speed is set to %i", sd->persStats->speed);

    serpentController->customData = sd;
    serpentController->entityType = ET_SERPENTCONTROLLER;

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

    serpentTJaw->model = gf3d_model_load("models/serpent_tjaw.model");
    serpentBJaw->model = gf3d_model_load("models/serpent_bjaw.model");
    serpentLure->model = gf3d_model_load("models/serpent_lure.model");
    lastSeg->model = gf3d_model_load("models/serpent_spikeseg.model");

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
    serpentTJaw->entityType = ET_SERPENTPART;
    serpentBJaw->entityType = ET_SERPENTPART;
    serpentLure->entityType = ET_LURE;
    lastSeg->entityType = ET_SERPENTPART;


    serpentController->children[0] = serpentTJaw;
    serpentController->children[1] = serpentBJaw;
    serpentController->children[2] = serpentLure;
    serpentController->children[3] = lastSeg;

    serpentController->think = serpent_think;
    serpentController->update = serpent_update;
    vector3d_copy(serpentController->position, position);
    serpentController->lastTickTime = SDL_GetTicks();
    serpentController->scale = vector3d(sd->size, sd->size, sd->size);

    vector3d_copy(serpentTJaw->position, serpentController->position);
    vector3d_copy(serpentBJaw->position, serpentController->position);
    vector3d_copy(serpentLure->position, serpentController->position);
    vector3d_copy(lastSeg->position, serpentController->position);

    serpentLure->bounds = gfc_sphere(serpentLure->position.x, serpentLure->position.y, serpentLure->position.z, 0.1);

    serpentLure->fearBounds = gfc_sphere(serpentLure->position.x, serpentLure->position.y, serpentLure->position.z, 25 + 5 * (persStats->lureStrength - 4));
    serpentLure->fearsomeness = 0;
    serpentController->fearBounds = gfc_sphere(serpentController->position.x, serpentController->position.y - sd->size, serpentController->position.z, sd->size * 2);
    serpentController->fearsomeness = sd->size;
    lastSeg->fearBounds = gfc_sphere(lastSeg->position.x, lastSeg->position.y, lastSeg->position.z, sd->size);
    lastSeg->fearsomeness = sd->size;

    serpentBJaw->fearsomeness = 0;
    serpentTJaw->fearsomeness = 0;

    lastSeg->localScale = vector3d(0.85, 0.85, 0.85);
    lastSeg->position.y -= serpentController->scale.y;
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
        newSeg->model = gf3d_model_load("models/serpent_spikeseg.model");
        vector3d_copy(newSeg->position, serpentController->position);
        vector3d_copy(newSeg->scale, serpentController->scale);
        newSeg->localScale = vector3d(0.85 - 0.05 * i, 0.85 - 0.05 * i, 0.85 - 0.05 * i);
        lastSeg->childCount++;
        lastSeg->children[lastSeg->childCount - 1] = newSeg;
        newSeg->parent = lastSeg;
        newSeg->position.y = lastSeg->position.y - lastSeg->scale.y;
        newSeg->followDist = lastSeg->scale.y;
        newSeg->think = serpent_segment_think;
        newSeg->entityType = ET_SERPENTPART;
        newSeg->fearBounds = gfc_sphere(newSeg->position.x, newSeg->position.y, newSeg->position.z, sd->size);
        newSeg->fearsomeness = sd->size;

        lastSeg = newSeg;
    }

    for (int i = 0; i < 8; i++) {
        serpent_add_segment(serpentController);
    }

    camera_set_offset(vector3d(0, 10, 10));

    return serpentController;
}


void serpent_think(Entity* self)
{
    if (!self)return;

    if (gfc_input_key_pressed("p")) {
        ((SerpentData*)(self->customData))->health = ((SerpentData*)(self->customData))->health -8;
        slog("Health is at %f%%.", ((SerpentData*)(self->customData))->health / ((SerpentData*)(self->customData))->healthMax);
    }
    ((SerpentData*)(self->customData))->still = 1;
    if (((SerpentData*)(self->customData))->snapping) {
        ((SerpentData*)(self->customData))->still = 0;
        float dx = 0;
        float dy = 0;
        float dz = 0;

        dx = sinf(self->rotation.z);
        dy = -cosf(self->rotation.z);
        dz = sinf(self->rotation.x);

        self->position.x -= dx * 0.03 * self->scale.y * ((SerpentData*)(self->customData))->persStats->speed;
        self->position.y -= dy * 0.03 * self->scale.y * ((SerpentData*)(self->customData))->persStats->speed;
        self->position.z += dz * 0.03 * self->scale.y * ((SerpentData*)(self->customData))->persStats->speed;
    }
    else if(!((SerpentData*)(self->customData))->luring) {
        int moving = 0;
        if (gfc_input_key_held("d")) {
            self->rotation.z -= 0.1 * GFC_DEGTORAD;
            moving += 1;
        }
        if (gfc_input_key_held("a")) {
            self->rotation.z += 0.1 * GFC_DEGTORAD;
            moving -= 1;
        }
        if (gfc_input_key_held("s")) {
            self->rotation.x += 0.1 * GFC_DEGTORAD;
            moving += 2;
        }
        if (gfc_input_key_held("w")) {
            self->rotation.x -= 0.1 * GFC_DEGTORAD;
            moving -= 2;
        }
        if (moving != 0) {
            ((SerpentData*)(self->customData))->still = 0;

            float dx = 0;
            float dy = 0;
            float dz = 0;

            dx = sinf(self->rotation.z);
            dy = -cosf(self->rotation.z);
            dz = sinf(self->rotation.x);

            self->position.x -= dx * 0.015 * self->scale.y * ((SerpentData*)(self->customData))->persStats->speed;
            self->position.y -= dy * 0.015 * self->scale.y * ((SerpentData*)(self->customData))->persStats->speed;
            self->position.z += dz * 0.015 * self->scale.y * ((SerpentData*)(self->customData))->persStats->speed;

            //slog("%f, %f", self->rotation.z, self->children[0]->rotation.z);
        }
    }
    vector3d_copy(self->fearBounds, self->position);
}
void serpent_update(Entity* self) {
    if (!self) return;

    if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_UP]) {
        ((SerpentData*)(self->customData))->zoom -= 0.005f;
    }
    if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_DOWN]) {
        ((SerpentData*)(self->customData))->zoom += 0.005f;
    }
    SDL_clamp(((SerpentData*)(self->customData))->zoom, 0.2f, 2);

    Vector3D camPos = self->position;
    Vector3D relPos = vector3d(0, ((SerpentData*)(self->customData))->zoom * -20 * self->scale.y, ((SerpentData*)(self->customData))->zoom * 20 * self->scale.z);

    if (((SerpentData*)(self->customData))->luring || ((SerpentData*)(self->customData))->snapping) {
        relPos.y = 0;
    }
    
    Vector3D camAngles = vector3d(
        -atanf(relPos.y / relPos.z) + GFC_PI,
        0,
        self->rotation.z
    );
    
    if (((SerpentData*)(self->customData))->luring || ((SerpentData*)(self->customData))->snapping) {
        camAngles.x += GFC_HALF_PI;
    }
    /*
    slog("camera yaw is set to %f.", camAngles.z);
    */
    camPos.x -= sinf(camAngles.z) * relPos.y;
    camPos.y += cosf(camAngles.z) * relPos.y;
    if (((SerpentData*)(self->customData))->luring || ((SerpentData*)(self->customData))->snapping) {
        camPos.z += 20 * ((SerpentData*)(self->customData))->zoom * self->scale.z;
    }
    else {
        camPos.z -= cosf(camAngles.x) * relPos.z;
    }

    //slog("camera pos is set to (%f, %f, %f).", camPos.x, camPos.y, camPos.z);

    gf3d_camera_set_position(camPos);
    gf3d_camera_set_rotation(camAngles);
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
    self->rotation.x = self->parent->rotation.x + self->customFloat * (self->rotSpeed > 0 ? -1 : 1);
    self->rotation.z = self->parent->rotation.z;
}
void crunch(void* fishy) {
    fish_free((Entity*)fishy, ((((Entity*)fishy))->entityType == ET_SWITCH) ? 1 : 0);
}
void serpent_lure_think(Entity* self) {
    if (!self || !self->parent)return;
    int showCoords = 0;
    if (gfc_input_key_held(" ")) {
        ((SerpentData*)(self->parent->customData))->luring = 1;
        if (((SerpentData*)(self->parent->customData))->lureOffset < 2 * self->scale.y && ((SerpentData*)(self->parent->customData))->lureOffset + 0.05 * self->scale.y >= 2 * self->scale.y)
            showCoords = 1;
        ((SerpentData*)(self->parent->customData))->lureOffset += 0.05 * self->scale.y;
        ((SerpentData*)(self->parent->customData))->lureOffset = MIN(((SerpentData*)(self->parent->customData))->lureOffset, 2 * self->scale.y);
    }
    else if (((SerpentData*)(self->parent->customData))->lureOffset != 0) {
        ((SerpentData*)(self->parent->customData))->luring = 0;
        ((SerpentData*)(self->parent->customData))->snapping = 1;
        ((SerpentData*)(self->parent->customData))->lureOffset -= 0.05 * self->scale.y;
        ((SerpentData*)(self->parent->customData))->lureOffset = MAX(((SerpentData*)(self->parent->customData))->lureOffset, 0);
    }
    else if (((SerpentData*)(self->parent->customData))->snapping == 1) {
        ((SerpentData*)(self->parent->customData))->snapping = 0;
        float totalNutrition = 0;
        int totalRewards[5] = { 0,0,0,0,0 };
        for (int i = 0; i < gfc_list_get_count(((SerpentData*)(self->parent->customData))->snappables); i++) {
            PreyData* pd = (PreyData*)(((Entity*)(gfc_list_get_nth(((SerpentData*)(self->parent->customData))->snappables, i)))->customData);
            totalNutrition += pd->nutrition;
            totalRewards[0] += pd->reward[0];
            totalRewards[1] += pd->reward[1];
            totalRewards[2] += pd->reward[2];
            totalRewards[3] += pd->reward[3];
            totalRewards[4] += pd->reward[4];
        }
        slog("Obtained %i silverium!", totalRewards[2]);
        (((SerpentData*)(self->parent->customData))->currencies)->mutagen += totalRewards[0];
        (((SerpentData*)(self->parent->customData))->currencies)->goldite += totalRewards[1];
        (((SerpentData*)(self->parent->customData))->currencies)->silverium += totalRewards[2];
        (((SerpentData*)(self->parent->customData))->currencies)->darkite += totalRewards[3];
        (((SerpentData*)(self->parent->customData))->currencies)->orbs += totalRewards[4];
        ((SerpentData*)(self->parent->customData))->hunger = max(((SerpentData*)(self->parent->customData))->hunger - totalNutrition, 0);
        gfc_list_foreach(((SerpentData*)(self->parent->customData))->snappables, &crunch);
        gfc_list_clear(((SerpentData*)(self->parent->customData))->snappables);
    }
    float dx = 0;
    float dy = 0;
    float dz = 0;

    dx = sinf(self->parent->rotation.z);
    dy = -cosf(self->parent->rotation.z);
    dz = sinf(self->parent->rotation.x);

    self->position.x = self->parent->position.x - dx * ((SerpentData*)(self->parent->customData))->lureOffset;
    self->position.y = self->parent->position.y - dy * ((SerpentData*)(self->parent->customData))->lureOffset;
    self->position.z = self->parent->position.z + dz * ((SerpentData*)(self->parent->customData))->lureOffset;
    self->rotation.x = self->parent->rotation.x;
    self->rotation.z = self->parent->rotation.z;

    if (showCoords) {
        slog("Lure is at  %f, %f, %f.", self->position.x, self->position.y, self->position.z);
    }

    vector3d_copy(self->fearBounds, self->position);
    vector3d_copy(self->bounds, self->position);
}
void serpent_segment_think(Entity* self) {
    Vector3D diff = vector3d(self->position.x - self->parent->position.x,self->position.y - self->parent->position.y,self->position.z - self->parent->position.z);
    vector3d_normalize(&diff);
    //self->rotation.x = acosf(-diff.z);
    //self->rotation.z = acosf(-sqrtf(diff.x * diff.x + diff.y * diff.y));
    
    //self->rotation = vector3d(vector3d_dot_product(diff, vector3d(1, 0, 0)), 0, vector3d_dot_product(diff, vector3d(0, 0, 1)));

    self->rotation = vector3d(vector_angle(sqrtf(diff.x * diff.x + diff.y * diff.y), diff.z) * -GFC_DEGTORAD + GFC_HALF_PI, 0, vector_angle(diff.x, diff.y) * GFC_DEGTORAD);
    
    self->position.x = self->parent->position.x + diff.x * self->scale.x;
    self->position.y = self->parent->position.y + diff.y * self->scale.x;
    self->position.z = self->parent->position.z + diff.z * self->scale.x;
    vector3d_copy(self->fearBounds, self->position);
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
    newSeg->model = gf3d_model_load("models/serpent_seg.model");
    vector3d_copy(newSeg->position, lastSeg->position);
    vector3d_copy(newSeg->scale, lastSeg->scale);
    newSeg->localScale = vector3d(0.6, 0.6, 0.6);
    lastSeg->childCount++;
    lastSeg->children[lastSeg->childCount - 1] = newSeg;
    newSeg->parent = lastSeg;
    newSeg->position.y = lastSeg->position.y - lastSeg->scale.y;
    newSeg->followDist = lastSeg->scale.y;
    newSeg->think = serpent_segment_think;
    newSeg->entityType = ET_SERPENTPART;
    newSeg->fearBounds = gfc_sphere(newSeg->position.x, newSeg->position.y, newSeg->position.z, ((SerpentData*)serpent->customData)->size);
    newSeg->fearsomeness = ((SerpentData*)serpent->customData)->size;
}

int* getUpgradeCosts(UpgrCats category, int currentLevel, int direction) {
    int costs[5];
    switch (category) {
    case Metabolism:
        if (direction == 1) {//upgrade
            switch (currentLevel)
            {
            case 1: // start minimum
                costs[Mutagen] = 10;
                costs[Goldite] = 5;
                costs[Silverium] = 0;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 2:
                costs[Mutagen] = 10;
                costs[Goldite] = 8;
                costs[Silverium] = 0;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 3:
                costs[Mutagen] = 10;
                costs[Goldite] = 15;
                costs[Silverium] = 0;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 4: // start neutral
                costs[Mutagen] = 20;
                costs[Goldite] = 25;
                costs[Silverium] = 0;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 5:
                costs[Mutagen] = 45;
                costs[Goldite] = 30;
                costs[Silverium] = 0;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 6:
                costs[Mutagen] = 100;
                costs[Goldite] = 60;
                costs[Silverium] = 0;
                costs[Darkite] = 3;
                costs[Orbs] = 0;
                break;
            case 7: // start maximum, DO NOT UPGRADE
                break;
            }
        }
        else if (direction == -1) {//downgrade
            switch (currentLevel)
            {
            case 1: // start minimum, DO NOT DOWNGRADE
                break;
            case 2:
                costs[Mutagen] = 100;
                costs[Goldite] = 0;
                costs[Silverium] = 60;
                costs[Darkite] = 3;
                costs[Orbs] = 0;
                break;
            case 3:
                costs[Mutagen] = 45;
                costs[Goldite] = 0;
                costs[Silverium] = 30;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 4: // start neutral
                costs[Mutagen] = 20;
                costs[Goldite] = 0;
                costs[Silverium] = 25;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 5:
                costs[Mutagen] = 10;
                costs[Goldite] = 0;
                costs[Silverium] = 15;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 6:
                costs[Mutagen] = 10;
                costs[Goldite] = 0;
                costs[Silverium] = 8;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 7: // start maximum
                costs[Mutagen] = 10;
                costs[Goldite] = 0;
                costs[Silverium] = 5;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            }
        }
        break;
    case LureStrength:
        if (direction == 1) {//upgrade
            switch (currentLevel)
            {
            case 1: // start minimum
                costs[Mutagen] = 10;
                costs[Goldite] = 5;
                costs[Silverium] = 0;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 2:
                costs[Mutagen] = 10;
                costs[Goldite] = 8;
                costs[Silverium] = 0;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 3:
                costs[Mutagen] = 10;
                costs[Goldite] = 15;
                costs[Silverium] = 0;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 4: // start neutral
                costs[Mutagen] = 20;
                costs[Goldite] = 25;
                costs[Silverium] = 0;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 5:
                costs[Mutagen] = 45;
                costs[Goldite] = 30;
                costs[Silverium] = 0;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 6:
                costs[Mutagen] = 100;
                costs[Goldite] = 60;
                costs[Silverium] = 0;
                costs[Darkite] = 3;
                costs[Orbs] = 0;
                break;
            case 7: // start maximum, DO NOT UPGRADE
                break;
            }
        }
        else if (direction == -1) {//downgrade
            switch (currentLevel)
            {
            case 1: // start minimum, DO NOT DOWNGRADE
                break;
            case 2:
                costs[Mutagen] = 100;
                costs[Goldite] = 0;
                costs[Silverium] = 60;
                costs[Darkite] = 3;
                costs[Orbs] = 0;
                break;
            case 3:
                costs[Mutagen] = 45;
                costs[Goldite] = 0;
                costs[Silverium] = 30;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 4: // start neutral
                costs[Mutagen] = 20;
                costs[Goldite] = 0;
                costs[Silverium] = 25;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 5:
                costs[Mutagen] = 10;
                costs[Goldite] = 0;
                costs[Silverium] = 15;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 6:
                costs[Mutagen] = 10;
                costs[Goldite] = 0;
                costs[Silverium] = 8;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            case 7: // start maximum
                costs[Mutagen] = 10;
                costs[Goldite] = 0;
                costs[Silverium] = 5;
                costs[Darkite] = 0;
                costs[Orbs] = 0;
                break;
            }
        }
        break;
    case Stealth:
        switch (currentLevel)
        {
        case 1: // start minimum
            costs[Mutagen] = 5;
            costs[Goldite] = 3;
            costs[Silverium] = 3;
            costs[Darkite] = 0;
            costs[Orbs] = 0;
            break;
        case 2:
            costs[Mutagen] = 20;
            costs[Goldite] = 8;
            costs[Silverium] = 8;
            costs[Darkite] = 0;
            costs[Orbs] = 0;
            break;
        case 3:
            costs[Mutagen] = 50;
            costs[Goldite] = 12;
            costs[Silverium] = 12;
            costs[Darkite] = 0;
            costs[Orbs] = 0;
            break;
        case 4: // start neutral
            costs[Mutagen] = 80;
            costs[Goldite] = 20;
            costs[Silverium] = 20;
            costs[Darkite] = 2;
            costs[Orbs] = 0;
            break;
        case 5: // start maximum, DO NOT UPGRADE
            break;
        }
        break;
    case Speed:
        switch (currentLevel)
        {
        case 1: // start minimum
            costs[Mutagen] = 5;
            costs[Goldite] = 3;
            costs[Silverium] = 3;
            costs[Darkite] = 0;
            costs[Orbs] = 0;
            break;
        case 2:
            costs[Mutagen] = 20;
            costs[Goldite] = 8;
            costs[Silverium] = 8;
            costs[Darkite] = 0;
            costs[Orbs] = 0;
            break;
        case 3:
            costs[Mutagen] = 50;
            costs[Goldite] = 12;
            costs[Silverium] = 12;
            costs[Darkite] = 0;
            costs[Orbs] = 0;
            break;
        case 4: // start neutral
            costs[Mutagen] = 80;
            costs[Goldite] = 20;
            costs[Silverium] = 20;
            costs[Darkite] = 2;
            costs[Orbs] = 0;
            break;
        case 5: // start maximum, DO NOT UPGRADE
            break;
        }
        break;
    case HeadStart:
        switch (currentLevel)
        {
        case 1: // start minimum
            costs[Mutagen] = 100;
            costs[Goldite] = 30;
            costs[Silverium] = 30;
            costs[Darkite] = 3;
            costs[Orbs] = 5;
            break;
        case 2:
            costs[Mutagen] = 350;
            costs[Goldite] = 50;
            costs[Silverium] = 50;
            costs[Darkite] = 8;
            costs[Orbs] = 10;
            break;
        case 3: // start maximum, DO NOT UPGRADE
            break;
        }
        break;
    }
    return costs;
}
int* getCanUpgrade(UpgrCats category, int currentLevel, int direction, PersCurrencies* wallet) {
    int* costs = getUpgradeCosts(category, currentLevel, direction);
    int enough[5];
    if (wallet->mutagen >= costs[Mutagen])
        enough[Mutagen] = 1;
    if (wallet->goldite >= costs[Goldite])
        enough[Goldite] = 1;
    if (wallet->silverium >= costs[Silverium])
        enough[Silverium] = 1;
    if (wallet->darkite >= costs[Darkite])
        enough[Darkite] = 1;
    if (wallet->orbs >= costs[Orbs])
        enough[Orbs] = 1;
    return enough;
}
void Upgrade(UpgrCats category, int currentLevel, int direction, PersCurrencies* wallet, Entity* serpent) {
    int* costs = getUpgradeCosts(category, currentLevel, direction);
    
}

/*eol@eof*/