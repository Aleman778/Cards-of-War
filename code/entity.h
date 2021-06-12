#ifndef entity_h_
#define entity_h_

#include "grid.h"

typedef struct
{
    int posX;
    int posY;
    bool underMouseCursor;
    bool selected;
    bool valid;
    bool playerControlled;
} entity_t;

entity_t* entity_init(entity_t* entity)
{
    entity->posX = 0;
    entity->posY = 0;
    entity->underMouseCursor = false;
    entity->selected = false;
    entity->valid = true;
    entity->playerControlled = false;

    return entity;
}

#endif
