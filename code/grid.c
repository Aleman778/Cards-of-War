#include <string.h>

void grid_init(grid_t* grid)
{
    memset(grid, GRID_NONE, sizeof(grid_t));
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

void grid_random_fill(grid_t* grid)
{
    for (int x = 0; x < GRID_SIZE_X; x++)
    {
        for (int y = 0; y < GRID_SIZE_Y; y++)
        {
            if (rand() % 100 < 20)
                grid->grid[x][y] = GRID_OBSTACLE;
        }
    }
}

void grid_compute_reachable_positions(grid_t* grid, int x, int y, int max_distance)
{
    if (max_distance < 0)
        return;
    
    if (grid->grid[x][y] != GRID_NONE)
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
            return abs(player->posX - gridX) + abs(player->posY - gridY) <= PLAYER_MOVE_DISTANCE && grid->grid[gridX][gridY] == GRID_NONE;
        } break;
        case MOVE_TYPE_HORIZONTAL: {
            if (player->posY != gridY)
                return false;
            
            for (int x = player->posX; x != gridX; x += signum(gridX - player->posX))
            {
                if (grid->grid[x][gridY] != GRID_NONE)
                    return false;
            }
            return grid->grid[gridX][gridY] == GRID_NONE;
        } break;
        case MOVE_TYPE_VERTICAL: {
            if (player->posX != gridX)
                return false;
            for (int y = player->posY; y != gridY; y += signum(gridY - player->posY))
            {
                if (grid->grid[gridX][y] != GRID_NONE)
                    return false;
            }
            return grid->grid[gridX][gridY] == GRID_NONE;
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
            
            switch (grid->grid[x][y])
            {
                case GRID_NONE:
                SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
                SDL_RenderDrawRect(renderer, &r);
                break;
                case GRID_OBSTACLE:
                SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
                SDL_RenderFillRect(renderer, &r);
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
                SDL_SetRenderDrawColor(renderer, 255, 200, 200, 255);
            else if (entity->underMouseCursor)
                SDL_SetRenderDrawColor(renderer, 255, 128, 128, 255);
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
        
        if (entity && entity->valid)
        {
            entity->underMouseCursor = false;
            if (input->mouse.x > entity->posX * GRID_ELEM_WIDTH && input->mouse.x < entity->posX * GRID_ELEM_WIDTH + GRID_ELEM_WIDTH)
            {
                if (input->mouse.y > entity->posY * GRID_ELEM_HEIGHT && input->mouse.y < entity->posY * GRID_ELEM_HEIGHT + GRID_ELEM_HEIGHT)
                {
                    entity->underMouseCursor = true;
                    memset(grid->valid_move_positions, 0, sizeof(grid->valid_move_positions));
                    grid_compute_reachable_positions(grid, entity->posX, entity->posY, PLAYER_MOVE_DISTANCE);
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
