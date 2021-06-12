#ifndef grid_h_
#define grid_h_

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "player.h"

#define GRID_ELEM_WIDTH 32
#define GRID_ELEM_HEIGHT GRID_ELEM_WIDTH

#define GRID_SIZE_X (WINDOW_WIDTH / GRID_ELEM_WIDTH)
#define GRID_SIZE_Y ((WINDOW_HEIGHT - (GRID_ELEM_HEIGHT * 5)) / GRID_ELEM_HEIGHT)

#define PLAYER_MOVE_DISTANCE 4

enum grid_objects
{
    GRID_NONE,
    //GRID_PLAYER,
};

enum move_type
{
    MOVE_TYPE_MAX_DIST,
    MOVE_TYPE_HORIZONTAL,
    MOVE_TYPE_VERTICAL,
    MOVE_TYPE_MAX,
};

typedef struct
{
    int grid[GRID_SIZE_X][GRID_SIZE_Y];
    player_t* player;
    int mouseGridX, mouseGridY;
    int move_type;
} grid_t;


void grid_init(grid_t* grid)
{
    memset(grid, GRID_NONE, sizeof(grid_t));
    //grid->grid[GRID_SIZE_X / 2][GRID_SIZE_Y / 2] = GRID_PLAYER;
}

bool grid_pos_within_player_range(grid_t* grid, int gridX, int gridY)
{
    if (gridX >= GRID_SIZE_X || gridY >= GRID_SIZE_Y)
        return false;

    switch (grid->move_type)
    {
        case MOVE_TYPE_MAX_DIST:
            return abs(grid->player->posX - gridX) + abs(grid->player->posY - gridY) <= PLAYER_MOVE_DISTANCE && grid->grid[gridX][gridY] == GRID_NONE;
            break;
        case MOVE_TYPE_HORIZONTAL:
            return grid->player->posX == gridX;
            break;
        case MOVE_TYPE_VERTICAL:
            return grid->player->posY == gridY;
            break;
    }
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
            }

            if (grid->player && (grid->player->underMouseCursor || grid->player->selected))
            {
                if (grid_pos_within_player_range(grid, x, y))
                {
                    if (grid->player->selected)
                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                    else
                        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
                    SDL_RenderDrawRect(renderer, &r);
                    if (grid->player->selected && x == grid->mouseGridX && y == grid->mouseGridY)
                        SDL_RenderFillRect(renderer, &r);
                }
            }
        }
    }
    
    if (grid->player)
    {
        player_t* player = grid->player;

        SDL_Rect player_rect;
        player_rect.x = player->posX * GRID_ELEM_WIDTH;
        player_rect.y = player->posY * GRID_ELEM_HEIGHT;
        player_rect.w = GRID_ELEM_WIDTH;
        player_rect.h = GRID_ELEM_HEIGHT;
        
        if (player->selected)
            SDL_SetRenderDrawColor(renderer, 255, 200, 200, 255);
        else if (player->underMouseCursor)
            SDL_SetRenderDrawColor(renderer, 255, 128, 128, 255);
        else
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &player_rect);
    }
}

void grid_onmouseevent(grid_t* grid, int x, int y, Uint32 event_type)
{
    grid->mouseGridX = x / GRID_ELEM_WIDTH;
    grid->mouseGridY = y / GRID_ELEM_HEIGHT;
    if (grid->player)
    {
        grid->player->underMouseCursor = false;
        if (x > grid->player->posX * GRID_ELEM_WIDTH && x < grid->player->posX * GRID_ELEM_WIDTH + GRID_ELEM_WIDTH)
        {
            if (y > grid->player->posY * GRID_ELEM_HEIGHT && y < grid->player->posY * GRID_ELEM_HEIGHT + GRID_ELEM_HEIGHT)
            {
                grid->player->underMouseCursor = true;
            }
        }

        if (event_type == SDL_MOUSEBUTTONDOWN)
        {
            if (grid->player->underMouseCursor)
            {
                grid->player->selected = true;
            }
            else if (grid->player->selected)
            {
                if (grid_pos_within_player_range(grid, grid->mouseGridX, grid->mouseGridY))
                {
                    grid->player->posX = grid->mouseGridX;
                    grid->player->posY = grid->mouseGridY;
                    grid->player->selected = false;
                }
                else
                {
                    grid->player->selected = false;
                }
            }
        }
    }
}

#endif
