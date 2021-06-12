#ifndef player_h_
#define player_h_

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct
{
    int posX;
    int posY;
    bool underMouseCursor;
    bool selected;
} player_t;

void player_init(player_t* player)
{
    player->posX = 0;
    player->posY = 0;
    player->underMouseCursor = false;
    player->selected = false;
}

#endif
