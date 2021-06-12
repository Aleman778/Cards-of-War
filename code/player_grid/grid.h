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

typedef struct
{
    int grid[GRID_SIZE_X][GRID_SIZE_Y];
} grid_t;


void grid_init(grid_t* grid)
{
    memset(grid, GRID_NONE, sizeof(grid_t));
    //grid->grid[GRID_SIZE_X / 2][GRID_SIZE_Y / 2] = GRID_PLAYER;
}

void grid_render(SDL_Renderer* renderer, grid_t* grid, player_t* player)
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
        }
    }

    if (player)
    {
        SDL_Rect player_rect;
        player_rect.x = player->posX * GRID_ELEM_WIDTH;
        player_rect.y = player->posY * GRID_ELEM_HEIGHT;
        player_rect.w = GRID_ELEM_WIDTH;
        player_rect.h = GRID_ELEM_HEIGHT;

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &player_rect);


        for (int x = fmax(player->posX - PLAYER_MOVE_DISTANCE, 0); x <= fmin(player->posX + PLAYER_MOVE_DISTANCE, GRID_SIZE_X); x++)
        {
            for (int y = fmax(player->posY - PLAYER_MOVE_DISTANCE, 0); y <= fmin(player->posY + PLAYER_MOVE_DISTANCE, GRID_SIZE_Y); y++)
            {
                if (abs(player->posX - x) + abs(player->posY - y) <= PLAYER_MOVE_DISTANCE && grid->grid[x][y] == GRID_NONE)
                {
                    SDL_Rect r2;
                    r2.x = x * GRID_ELEM_WIDTH;
                    r2.y = y * GRID_ELEM_HEIGHT;
                    r2.w = GRID_ELEM_WIDTH;
                    r2.h = GRID_ELEM_HEIGHT;

                    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
                    SDL_RenderDrawRect(renderer, &r2);
                }
            }
        }
    }
}

#endif
