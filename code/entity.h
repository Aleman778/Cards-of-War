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
    f32 health;
    f32 maxHealth;
} entity_t;

entity_t* entity_init(entity_t* entity)
{
    entity->posX = 0;
    entity->posY = 0;
    entity->underMouseCursor = false;
    entity->selected = false;
    entity->valid = true;
    entity->playerControlled = false;
    entity->health = 100;
    entity->maxHealth = entity->health;
    return entity;
}

struct grid;

void enemy_perform_random_move(entity_t* enemy, struct grid* grid);
void render_entity(SDL_Renderer* renderer, entity_t* entity);

#endif
