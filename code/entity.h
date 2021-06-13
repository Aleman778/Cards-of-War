#ifndef entity_h_
#define entity_h_

#include "grid.h"

typedef struct
{
    int posX;
    int posY;
    int targetPosX;
    int targetPosY;
    bool underMouseCursor;
    bool selected;
    bool valid;
    bool playerControlled;
    f32 health;
    f32 maxHealth;
    int (*valid_move_positions)[GRID_SIZE_Y];
    Card lastCard;
} entity_t;

entity_t* entity_init(entity_t* entity)
{
    entity->posX = 0;
    entity->posY = 0;
    entity->targetPosX = 0;
    entity->targetPosY = 0;
    entity->underMouseCursor = false;
    entity->selected = false;
    entity->valid = true;
    entity->playerControlled = false;
    entity->health = 30;
    entity->maxHealth = entity->health;
    entity->valid_move_positions = malloc(GRID_SIZE_X * GRID_SIZE_Y * sizeof(int));
    return entity;
}

struct grid;

void enemy_random_chase_move(entity_t* enemy, entity_t* player, struct grid* grid);
void enemy_perform_random_move(entity_t* enemy, struct grid* grid);
void render_entity(SDL_Renderer* renderer, entity_t* entity);
void update_entity(entity_t* entity);

#endif
