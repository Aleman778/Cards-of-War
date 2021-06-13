#include <SDL.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "grid.h"
#include "entity.h"

void grid_init(struct grid* grid)
{
    memset(grid, 0, sizeof(struct grid));
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

// Dir: up = 1, right = 2, down = 3, left = 4
void _grid_compute_reachable_positions(struct grid* grid, entity_t* entity, int x, int y, int dir, int max_distance)
{
    if (max_distance < 0 || entity->valid_move_positions[x][y] != 0)
        return;
    
    if (!grid_pos_walkable(grid->grid[x][y]))
    {
        entity->valid_move_positions[x][y] = 0;
        return;
    }
    
    entity->valid_move_positions[x][y] = dir;
    
    _grid_compute_reachable_positions(grid, entity, grid_limit_x(x + 1), grid_limit_y(y), 2, max_distance - 1);
    _grid_compute_reachable_positions(grid, entity, grid_limit_x(x), grid_limit_y(y + 1), 3, max_distance - 1);
    _grid_compute_reachable_positions(grid, entity, grid_limit_x(x - 1), grid_limit_y(y), 4, max_distance - 1);
    _grid_compute_reachable_positions(grid, entity, grid_limit_x(x), grid_limit_y(y - 1), 1, max_distance - 1);
}

void grid_compute_reachable_positions(struct grid* grid, entity_t* entity, int x, int y, int max_distance)
{
    _grid_compute_reachable_positions(grid, entity, x, y, 0, max_distance);
    
}

int signum(int x)
{
    if (x < 0)
        return -1;
    if (x > 0)
        return 1;
    return 0;
}

bool grid_pos_within_player_range(struct grid* grid, entity_t* player, int gridX, int gridY)
{
    if (gridX >= GRID_SIZE_X || gridY >= GRID_SIZE_Y)
        return false;
    
    if (grid->card) {
        switch (grid->card->type)
        {
            case CardType_Movement_Free: {
                return player->valid_move_positions[gridX][gridY] > 0;
            } break;
            case CardType_Movement_Horizontal: {
                if (player->posY != gridY)
                    return false;
                
                for (int x = player->posX; x != gridX; x += signum(gridX - player->posX))
                {
                    if (!grid_pos_walkable(grid->grid[x][gridY]))
                        return false;
                }
                return grid_pos_walkable(grid->grid[gridX][gridY]);
            } break;
            case CardType_Movement_Vertical: {
                if (player->posX != gridX)
                    return false;
                for (int y = player->posY; y != gridY; y += signum(gridY - player->posY))
                {
                    if (!grid_pos_walkable(grid->grid[gridX][y]))
                        return false;
                }
                return grid_pos_walkable(grid->grid[gridX][gridY]);
            } break;
            
            case CardType_Attack_Laser: {
                if (player->posX != gridX && player->posY != gridY)
                    return false;
                for (int x = player->posX; x != gridX; x += signum(gridX - player->posX))
                {
                    if (!grid_pos_walkable(grid->grid[x][gridY]))
                        return false;
                }
                for (int y = player->posY; y != gridY; y += signum(gridY - player->posY))
                {
                    if (!grid_pos_walkable(grid->grid[gridX][y]))
                        return false;
                }
                return grid_pos_walkable(grid->grid[gridX][gridY]);
            } break;
            
            case CardType_Attack_Cannon: {
                return abs(player->posX - gridX) + abs(player->posY - gridY) <= PLAYER_MOVE_DISTANCE && grid_pos_walkable(grid->grid[gridX][gridY]);
                //return grid->valid_move_positions[gridX][gridY] > 0;
            } break;
            case CardType_Attack_Blast: {
                return player->valid_move_positions[gridX][gridY] > 0;
            } break;
        }
    }
    
    return false;
}

void grid_render(SDL_Renderer* renderer, struct grid* grid)
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
                        if (entity->selected) {
                            if (grid->card && grid->card->type >= CardType_Attack_First) {
                                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                            } else {
                                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                            }
                        } else {
                            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
                        }
                        SDL_RenderDrawRect(renderer, &r);
                        if (entity->selected && x == grid->mouseGridX && y == grid->mouseGridY
                            && grid->card && (grid->card->type <= CardType_Movement_Last ||
                                              grid->card->type == CardType_Attack_Cannon)) {
                            SDL_RenderFillRect(renderer, &r);
                        }
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
            render_entity(renderer, entity);
        }
    }
}

bool // NOTE(alexander): indicates a valid move was made
grid_perform_action(struct grid* grid, Input* input, Player_Hand* player, entity_t* entity) {
    grid->mouseGridX = (s32) input->mouse.x / GRID_ELEM_WIDTH;
    grid->mouseGridY = (s32) input->mouse.y / GRID_ELEM_HEIGHT;
    
    bool is_valid_move = false;
    if (player->is_selected && grid->card) {
        if (button_was_pressed(&input->mouse_buttons[SDL_BUTTON_LEFT])) {
            if (input->mouse.y < WINDOW_HEIGHT - 40) {
                bool is_within_grid = grid_pos_within_player_range(grid, entity, grid->mouseGridX, grid->mouseGridY);
                
                switch (grid->card->type) {
                    case CardType_Movement_Free:
                    case CardType_Movement_Horizontal:
                    case CardType_Movement_Vertical: {
                        is_valid_move = is_within_grid;
                        if (is_valid_move) {
                            entity->targetPosX = grid->mouseGridX;
                            entity->targetPosY = grid->mouseGridY;
                        }
                    } break;
                    
                    case CardType_Attack_Laser: {
                        is_valid_move = true;
                        for (int i = 0; i < MAX_ENTITIES; i++)
                        {
                            entity_t* enemy = &grid->entities[i];
                            if (!enemy->playerControlled &&
                                grid_pos_within_player_range(grid, entity, enemy->posX, enemy->posY)) {
                                enemy->health -= (f32) grid->card->attack;
                                // TODO(alexander): defeat enemy, health <= 0
                            }
                        }
                    } break;
                    
                    case CardType_Attack_Cannon: {
                        is_valid_move = is_within_grid;
                        for (int i = 0; i < MAX_ENTITIES; i++)
                        {
                            entity_t* enemy = &grid->entities[i];
                            if (enemy->posX == grid->mouseGridX &&
                                enemy->posY == grid->mouseGridY) {
                                enemy->health -= (f32) grid->card->attack;
                                // TODO(alexander): defeat enemy, health <= 0
                            }
                        }
                    } break;
                    
                    case CardType_Attack_Blast: {
                        is_valid_move = true;
                        for (int i = 0; i < MAX_ENTITIES; i++)
                        {
                            entity_t* enemy = &grid->entities[i];
                            if (!enemy->playerControlled &&
                                grid_pos_within_player_range(grid, entity, enemy->posX, enemy->posY)) {
                                enemy->health -= (f32) grid->card->attack;
                                // TODO(alexander): defeat enemy, health <= 0
                            }
                        }
                    } break;
                }
                
                if (is_valid_move) {
                    // Discard the used card
                    int cards_right = player->num_cards - player->selected_card;
                    if (cards_right > 0) {
                        memmove(player->cards + player->selected_card, 
                                player->cards + (player->selected_card + 1),
                                cards_right * sizeof(Card));
                    } 
                    
                    player->num_cards--;
                    
                    state.type = GameState_Animation;
                    state.next_state = GameState_EnemyTurn;
                }
            }
            entity->selected = false;
            player->is_selected = false;
            grid->card = 0;
        }
    }
    
    return is_valid_move;
}

void grid_update_entities(struct grid* grid)
{
    for (int entityIndex = 0; entityIndex < MAX_ENTITIES; entityIndex++)
    {
        entity_t* entity = &grid->entities[entityIndex];
        
        if (entity && entity->valid)
        {
            update_entity(entity);
        }
    }
}
