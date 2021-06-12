#include <SDL.h>

#include "entity.h"
#include "grid.h"
#include "vecmath.h"

/*
void enemy_take_best_action(entity_t* enemy, entity_t* player, struct grid* grid)
{
    // Maybe implement some logic here later?
}
*/

void enemy_perform_random_move(entity_t* enemy, struct grid* grid)
{
    memset(grid->valid_move_positions, 0, sizeof(grid->valid_move_positions));
    grid_compute_reachable_positions(grid, enemy->posX, enemy->posY, PLAYER_MOVE_DISTANCE);

    for (int i = 0; i < 1000; i++)
    {
        int index = rand() % array_count(grid->valid_move_positions);
        if (grid->valid_move_positions[index])
        {
            v2 pos = index_to_pos(index, GRID_SIZE_X);
            enemy->posX = pos.x;
            enemy->posY = pos.y;

            break;
        }
    }

    enemy->health -= 25;
}

void render_entity_health_bar(SDL_Renderer* renderer, entity_t* entity)
{
    const int bar_width = 50;
    const int bar_height = 8;

    int x = fmax((entity->posX * GRID_ELEM_WIDTH + GRID_ELEM_WIDTH / 2) - bar_width / 2, 0);
    int y = fmax(entity->posY * GRID_ELEM_HEIGHT - bar_height - 8, 0);

    SDL_Rect background;
    background.x = x;
    background.y = y;
    background.w = bar_width;
    background.h = bar_height;

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &background);

    SDL_Rect foreground;
    foreground.x = x;
    foreground.y = y;
    foreground.w = bar_width * (entity->health / entity->maxHealth);
    foreground.h = bar_height;

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &foreground);

    if (entity->health <= 0)
        entity->valid = false;
}

void render_entity(SDL_Renderer* renderer, entity_t* entity)
{
    SDL_Rect entity_rect;
    entity_rect.x = entity->posX * GRID_ELEM_WIDTH;
    entity_rect.y = entity->posY * GRID_ELEM_HEIGHT;
    entity_rect.w = GRID_ELEM_WIDTH;
    entity_rect.h = GRID_ELEM_HEIGHT;

    if (entity->selected)
        SDL_SetRenderDrawColor(renderer, 200, 200, 255, 255);
    else if (entity->underMouseCursor)
        SDL_SetRenderDrawColor(renderer, 128, 128, 255, 255);
    else if (entity->playerControlled)
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    else
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &entity_rect);

    render_entity_health_bar(renderer, entity);
}