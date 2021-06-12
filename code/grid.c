#include <SDL.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "grid.h"

void grid_init(grid_t* grid)
{
    memset(grid, 0, sizeof(grid_t));
    //grid->grid[GRID_SIZE_X / 2][GRID_SIZE_Y / 2] = GRID_PLAYER;
}

int grid_limit_x(int x)
{
    return (int) fmax(0, fmin(GRID_SIZE_X - 1, x));
}

int grid_limit_y(int y)
{
    return (int)  fmax(0, fmin(GRID_SIZE_Y - 1, y));
}

bool grid_pos_walkable(int grid_val)
{
    switch (grid_val)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 10:
    case 11:
    case 12:
    case 13:
    case 19:
    case 20:
    case 21:
    case 22:
    case 28:
    case 29:
    case 30:
    case 31:
        return true;
        break;

    default:
        return false;
        break;
    }
}

void grid_compute_reachable_positions(grid_t* grid, int x, int y, int max_distance)
{
    if (max_distance < 0)
        return;
    
    if (!grid_pos_walkable(grid->grid[x][y]))
    {
        grid->valid_move_positions[x][y] = 0;
        return;
    }
    
    grid->valid_move_positions[x][y] = 1;
    
    grid_compute_reachable_positions(grid, grid_limit_x(x + 1), grid_limit_y(y), max_distance - 1);
    grid_compute_reachable_positions(grid, grid_limit_x(x), grid_limit_y(y + 1), max_distance - 1);
    grid_compute_reachable_positions(grid, grid_limit_x(x - 1), grid_limit_y(y), max_distance - 1);
    grid_compute_reachable_positions(grid, grid_limit_x(x), grid_limit_y(y - 1), max_distance - 1);
}

int signum(int x)
{
    if (x < 0)
        return -1;
    if (x > 0)
        return 1;
    return 0;
}

bool grid_pos_within_player_range(grid_t* grid, entity_t* player, int gridX, int gridY)
{
    if (gridX >= GRID_SIZE_X || gridY >= GRID_SIZE_Y)
        return false;
    
    switch (grid->move_type)
    {
        case MOVE_TYPE_IGNORE_OBSTACLES: {
            return abs(player->posX - gridX) + abs(player->posY - gridY) <= PLAYER_MOVE_DISTANCE && grid_pos_walkable(grid->grid[gridX][gridY]);
        } break;
        case MOVE_TYPE_HORIZONTAL: {
            if (player->posY != gridY)
                return false;
            
            for (int x = player->posX; x != gridX; x += signum(gridX - player->posX))
            {
                if (!grid_pos_walkable(grid->grid[x][gridY]))
                    return false;
            }
            return grid_pos_walkable(grid->grid[gridX][gridY]);
        } break;
        case MOVE_TYPE_VERTICAL: {
            if (player->posX != gridX)
                return false;
            for (int y = player->posY; y != gridY; y += signum(gridY - player->posY))
            {
                if (!grid_pos_walkable(grid->grid[gridX][y]))
                    return false;
            }
            return grid_pos_walkable(grid->grid[gridX][gridY]);
        } break;
        case MOVE_TYPE_BREADTH_FIRST: {
            return grid->valid_move_positions[gridX][gridY] > 0;
        } break;
    }
    
    return false;
}

void grid_render(SDL_Renderer* renderer, grid_t* grid)
{
    
    SDL_Rect grid_border;
    grid_border.x = 0;
    grid_border.y = 0;
    grid_border.w = GRID_ELEM_WIDTH * GRID_SIZE_X;
    grid_border.h = GRID_ELEM_HEIGHT * GRID_SIZE_Y;
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &grid_border);
    
    for (int x = 0; x < GRID_SIZE_X; x++)
    {
        for (int y = 0; y < GRID_SIZE_Y; y++)
        {
            SDL_Rect r;
            r.x = x * GRID_ELEM_WIDTH;
            r.y = y * GRID_ELEM_HEIGHT;
            r.w = GRID_ELEM_WIDTH;
            r.h = GRID_ELEM_HEIGHT;
            
            
            int tile_index = grid->grid[x][y];
            if (tile_index > 0) {
                tile_index--; // NOTE(alexander): tiled uses 0 as null tile, first tile is 1
                
                // Draw tile
                SDL_Rect tr;
                tr.x = (tile_index % (grid->tileset_width / 16)) * 16;
                tr.y = (tile_index / (grid->tileset_width / 16)) * 16;
                tr.w = 16;
                tr.h = 16;
                SDL_RenderCopy(renderer, grid->tileset, &tr, &r);
            }
            
            
            for (int entityIndex = 0; entityIndex < MAX_ENTITIES; entityIndex++)
            {
                entity_t* entity = &grid->entities[entityIndex];
                
                if (entity && entity->valid && (entity->underMouseCursor || entity->selected))
                {
                    if (grid_pos_within_player_range(grid, entity, x, y))
                    {
                        if (entity->selected)
                            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                        else
                            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
                        SDL_RenderDrawRect(renderer, &r);
                        if (entity->selected && x == grid->mouseGridX && y == grid->mouseGridY)
                            SDL_RenderFillRect(renderer, &r);
                    }
                }
            }
        }
    }
    
    for (int entityIndex = 0; entityIndex < MAX_ENTITIES; entityIndex++)
    {
        entity_t* entity = &grid->entities[entityIndex];
        
        if (entity && entity->valid)
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
        }
    }
}

void grid_move_player(grid_t* grid, Input* input)
{
    grid->mouseGridX = (s32) input->mouse.x / GRID_ELEM_WIDTH;
    grid->mouseGridY = (s32) input->mouse.y / GRID_ELEM_HEIGHT;
    
    for (int entityIndex = 0; entityIndex < MAX_ENTITIES; entityIndex++)
    {
        entity_t* entity = &grid->entities[entityIndex];
        
        if (entity && entity->valid && entity->playerControlled)
        {
            entity->underMouseCursor = false;
            if (input->mouse.x > entity->posX * GRID_ELEM_WIDTH && input->mouse.x < entity->posX * GRID_ELEM_WIDTH + GRID_ELEM_WIDTH)
            {
                if (input->mouse.y > entity->posY * GRID_ELEM_HEIGHT && input->mouse.y < entity->posY * GRID_ELEM_HEIGHT + GRID_ELEM_HEIGHT)
                {
                    entity->underMouseCursor = true;
                    
                    bool anotherEntitySelected = false;
                    for (int entityIndex2 = 0; entityIndex2 < MAX_ENTITIES; entityIndex2++)
                    {
                        entity_t* entity2 = &grid->entities[entityIndex2];
                        
                        if (entity2 && entity2->valid && entity2->selected)
                            anotherEntitySelected = true;
                    }
                    
                    if (!anotherEntitySelected)
                    {
                        memset(grid->valid_move_positions, 0, sizeof(grid->valid_move_positions));
                        grid_compute_reachable_positions(grid, entity->posX, entity->posY, PLAYER_MOVE_DISTANCE);
                    }
                }
            }
            
            if (button_was_pressed(&input->mouse_buttons[SDL_BUTTON_LEFT]))
            {
                if (entity->underMouseCursor)
                {
                    entity->selected = true;
                    memset(grid->valid_move_positions, 0, sizeof(grid->valid_move_positions));
                    grid_compute_reachable_positions(grid, entity->posX, entity->posY, PLAYER_MOVE_DISTANCE);
                }
                else if (entity->selected)
                {
                    if (grid_pos_within_player_range(grid, entity, grid->mouseGridX, grid->mouseGridY))
                    {
                        entity->posX = grid->mouseGridX;
                        entity->posY = grid->mouseGridY;
                        memset(grid->valid_move_positions, 0, sizeof(grid->valid_move_positions));
                        grid_compute_reachable_positions(grid, entity->posX, entity->posY, PLAYER_MOVE_DISTANCE);
                    }
                    
                    entity->selected = false;
                }
            }
        }
    }
}
