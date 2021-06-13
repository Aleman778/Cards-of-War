#include <SDL.h>

#include "entity.h"
#include "grid.h"
#include "vecmath.h"

// Attempt to move closer to the player
void enemy_random_chase_move(entity_t* enemy, entity_t* player)
{
    memset(enemy->valid_move_positions, 0, GRID_SIZE_BYTES);
    grid_compute_reachable_positions(enemy->grid, enemy, enemy->posX, enemy->posY, PLAYER_MOVE_DISTANCE);
    bool hasMoved = false;
    
    for (int i = 0; i < 500; i++)
    {
        int x = rand() % GRID_SIZE_X;
        int y = rand() % GRID_SIZE_Y;
        
        if (enemy->valid_move_positions[x][y] && !(x == enemy->posX && y == enemy->posY))
        {
            v2 movePos = vec2((f32) x, (f32) y);
            v2 pos = vec2((f32) enemy->posX, (f32) enemy->posY);
            v2 playerPos = vec2((f32) player->posX, (f32) player->posY);
            
            if (vec2_length_sq(vec2_sub(movePos, playerPos)) > vec2_length_sq(vec2_sub(pos, playerPos)))
                continue;
            
            enemy->targetPosX = (s32) movePos.x;
            enemy->targetPosY = (s32) movePos.y;
            
            hasMoved = true;
            break;
        }
    }
    
    if (!hasMoved)
    {
        for (int i = 0; i < 500; i++)
        {
            int x = rand() % GRID_SIZE_X;
            int y = rand() % GRID_SIZE_Y;
            
            if (enemy->valid_move_positions[x][y])
            {
                enemy->targetPosX = x;
                enemy->targetPosY = y;
                
                break;
            }
        }
    }
}


void enemy_perform_random_move(entity_t* enemy)
{
    memset(enemy->valid_move_positions, 0, GRID_SIZE_BYTES);
    grid_compute_reachable_positions(enemy->grid, enemy, enemy->posX, enemy->posY, PLAYER_MOVE_DISTANCE);
    
    for (int i = 0; i < 1000; i++)
    {
        int x = rand() % GRID_SIZE_X;
        int y = rand() % GRID_SIZE_Y;
        
        if (enemy->valid_move_positions[x][y])
        {
            enemy->targetPosX = (s32) x;
            enemy->targetPosY = (s32) y;
            
            break;
        }
    }
    
    enemy->health -= 25;
}

void render_entity_health_bar(SDL_Renderer* renderer, entity_t* entity)
{
    const int bar_width = 50;
    const int bar_height = 8;
    
    s32 x = (s32) fmax((entity->posX * GRID_ELEM_WIDTH + GRID_ELEM_WIDTH / 2) - bar_width / 2, 0);
    s32 y = (s32) fmax(entity->posY * GRID_ELEM_HEIGHT - bar_height - 8, 0);
    
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
    foreground.w = (s32) (bar_width * (entity->health / entity->maxHealth));
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
    
    if (entity->tileIDs[entity->direction] > 0) {
        int tile_index = entity->tileIDs[entity->direction] - 1; // NOTE(alexander): tiled uses 0 as null tile, first tile is 1
        
        // Draw tile
        SDL_Rect tr;
        tr.x = (tile_index % (entity->grid->tileset_width / 16)) * 16;
        tr.y = (tile_index / (entity->grid->tileset_width / 16)) * 16;
        tr.w = 16;
        tr.h = 16;
        SDL_RenderCopy(renderer, entity->grid->tileset, &tr, &entity_rect);
    }
    
    // if (entity->selected)
    //     SDL_SetRenderDrawColor(renderer, 200, 200, 255, 255);
    // else if (entity->underMouseCursor)
    //     SDL_SetRenderDrawColor(renderer, 128, 128, 255, 255);
    // else if (entity->playerControlled)
    //     SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    // else
    //     SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    // SDL_RenderFillRect(renderer, &entity_rect);
    
    render_entity_health_bar(renderer, entity);
    
    if (entity->targetPosX != entity->posX || entity->targetPosY != entity->posY)
    {
        if (state.current_entity == entity)
        {
            v2 position = vec2(0.0f, 0.0f);
            
            if (entity->playerControlled)
            {
                position.x = 0.0f;
                if (entity->posY <= 5 && entity->posX < 4)
                    position.y = 400.0f - CARD_HEIGHT;
            }
            else
            {
                position.x = WINDOW_WIDTH - CARD_WIDTH;
                if (entity->posY <= 5 && entity->posX >= GRID_SIZE_X - 4)
                    position.y = 400.0f - CARD_HEIGHT;
            }
            
            draw_card(renderer, &entity->lastCard, entity->grid, position);
        }
    }
}

int find_direction_to_target(entity_t* entity, int targetX, int targetY)
{
    int dir = 0;
    int counter = 0;
    
    if (entity->lastCard.type == CardType_Movement_Horizontal)
    {
        if (targetX > entity->posX)
            return 2;
        else
            return 4;
    }
    else if (entity->lastCard.type == CardType_Movement_Vertical)
    {
        if (targetY > entity->posY)
            return 3;
        else
            return 1;
    }
    
    while ((entity->posX != targetX || entity->posY != targetY))
    {
        counter++;
        if (counter > 1000)
        {
            printf("Failed to find direction to target X = %d, Y = %d\n", targetX, targetY);
            return 0;
        }
        
        dir = entity->valid_move_positions[targetX][targetY];
        switch (dir)
        {
            case 1:
            targetY++;
            break;
            case 2:
            targetX--;
            break;
            case 3:
            targetY--;
            break;
            case 4:
            targetX++;
            break;
            
            default:
            return 0;
        }
    }
    return dir;
}

void entity_move_forward(entity_t* entity)
{
    switch (entity->direction)
    {
        case 1:
        entity->posY--;
        break;
        case 2:
        entity->posX++;
        break;
        case 3:
        entity->posY++;
        break;
        case 4:
        entity->posX--;
        break;
        
        default:
        break;
    }
}

void update_entity(entity_t* entity)
{
    static u32 lastUpdateTime = 0;
    
    if (SDL_GetTicks() - lastUpdateTime > 250)
    {
        if (entity->posX != entity->targetPosX || entity->posY != entity->targetPosY)
        {
            state.type = GameState_Animation;
            state.current_entity = entity;
            
            lastUpdateTime = SDL_GetTicks();
            
            entity->direction = find_direction_to_target(entity, entity->targetPosX, entity->targetPosY);
            entity_move_forward(entity);
        }
        else if (state.type == GameState_Animation)
        {
            state.type = GameState_Select_Cards;
            state.next_state = GameState_Turn;
        }
    }
}
