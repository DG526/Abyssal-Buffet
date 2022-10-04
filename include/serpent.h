#ifndef __SERPENT_H__
#define __SERPENT_H__

#include "entity.h"

/**
 * @brief Create a new serpent entity
 * @param position where to spawn the serpent at
 * @return NULL on error, or an serpent entity pointer on success
 */
Entity* serpent_new(Vector3D position);
Entity* serpent_spikeseg_new(Entity* par);
Entity* serpent_seg_new(Entity* par);



#endif