#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"

#include "gf3d_model.h"

typedef enum {
    ET_DEFAULT = 0,
    ET_SERPENTCONTROLLER,
    ET_SERPENTTJAW,
    ET_SERPENTBJAW,
    ET_SERPENTLURE,
    ET_SERPENTSEG,
    ET_PREY,
    ET_SWITCH,
    ET_PREDATOR
}EntityType;

typedef struct Entity_S
{
    Uint8       _inuse;     /**<keeps track of memory usage*/
    Matrix4     modelMat;   /**<orientation matrix for the model*/
    Model      *model;      /**<pointer to the entity model to draw  (optional)*/
    void       (*think)(struct Entity_S *self); /**<pointer to the think function*/
    void       (*update)(struct Entity_S *self); /**<pointer to the update function*/
    void       (*draw)(struct Entity_S *self); /**<pointer to an optional extra draw funciton*/
    void       (*damage)(struct Entity_S *self, float damage, struct Entity_S *inflictor); /**<pointer to the think function*/
    void       (*onDeath)(struct Entity_S *self); /**<pointer to an funciton to call when the entity dies*/
    
    Vector3D    position;  
    Vector3D    velocity;
    Vector3D    acceleration;
    
    Uint32      randomSeed;
    Uint32      lastTickTime;

    EntityType entityType;

    struct Entity_S* parent;
    float followDist;
    Uint32  childCount;
    struct Entity_S* children[16];

    float speed;
    float rotSpeed;

    float customFloat;
    
    Vector3D    scale;
    Vector3D    rotation;

    Vector3D    localScale;
    Vector3D    localRotation;
    
    Uint32      health;     /**<entity dies when it reaches zero*/
    // WHATEVER ELSE WE MIGHT NEED FOR ENTITIES
    struct Entity_S *target;    /**<entity to target for weapons / ai*/
    
    void *customData;   /**<IF an entity needs to keep track of extra data, we can do it here*/
}Entity;

/**
 * @brief initializes the entity subsystem
 * @param maxEntities the limit on number of entities that can exist at the same time
 */
void entity_system_init(Uint32 maxEntities);

/**
 * @brief provide a pointer to a new empty entity
 * @return NULL on error or a valid entity pointer otherwise
 */
Entity *entity_new();

/**
 * @brief free a previously created entity from memory
 * @param self the entity in question
 */
void entity_free(Entity *self);


/**
 * @brief Draw an entity in the current frame
 * @param self the entity in question
 */
void entity_draw(Entity *self);

/**
 * @brief draw ALL active entities
 */
void entity_draw_all();

/**
 * @brief Call an entity's think function if it exists
 * @param self the entity in question
 */
void entity_think(Entity *self);

/**
 * @brief run the think functions for ALL active entities
 */
void entity_think_all();

/**
 * @brief run the update functions for ALL active entities
 */
void entity_update_all();

#endif
