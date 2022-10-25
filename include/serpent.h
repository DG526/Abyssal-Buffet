#ifndef __SERPENT_H__
#define __SERPENT_H__

#include "entity.h"
#include "gf3d_sprite.h"


typedef struct {
    int metobolism; //1-7, starts at 4.
    int lureStrength; //1-7, starts at 4.
    int stealth; //1-5, starts at 1.
    int speed; //1-5, starts at 1.
    int headStart; //1-3, starts at 1.

}SerpentPersStats;

typedef struct {
    float health, healthMax;
    int size; //determines what can be eaten
    int length; //how many segments
    float hunger; //when it reaches 1, serpent loses health
    float hungerRate;
    Sprite* hpBar;
    SerpentPersStats* persStats;
}SerpentData;
/**
 * @brief Create a new serpent entity
 * @param position where to spawn the serpent at
 * @return NULL on error, or an serpent entity pointer on success
 */
Entity* serpent_new(Vector3D position, SerpentPersStats* persStats);
Entity* serpent_spikeseg_new(Entity* par);
Entity* serpent_seg_new(Entity* par);



#endif