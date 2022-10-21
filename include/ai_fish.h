#ifndef __FISH_H__
#define __FISH_H__

#include "entity.h"

/**
 * @brief Create a new random prey entity
 * @param position where to spawn the serpent at
 * @param sizeMin the minimum size prey to spawn
 * @param sizeMax the maximum size prey to spawn
 * @return NULL on error, or an serpent entity pointer on success
 */
Entity* prey_new(Vector3D position, float sizeMin, float sizeMax);

Entity* oddfish_new(Vector3D position, float size);


#endif