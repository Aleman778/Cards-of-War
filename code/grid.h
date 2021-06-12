#ifndef grid_h_
#define grid_h_

#define GRID_ELEM_WIDTH 32
#define GRID_ELEM_HEIGHT GRID_ELEM_WIDTH

#define GRID_SIZE_X (WINDOW_WIDTH / GRID_ELEM_WIDTH)
#define GRID_SIZE_Y (WINDOW_HEIGHT / GRID_ELEM_HEIGHT)

#define PLAYER_MOVE_DISTANCE 4


#include "entity.h"
#include "vecmath.h"

enum move_type
{
    MOVE_TYPE_BREADTH_FIRST,
    MOVE_TYPE_HORIZONTAL,
    MOVE_TYPE_VERTICAL,
    MOVE_TYPE_IGNORE_OBSTACLES,
    MOVE_TYPE_MAX,
};

typedef struct
{
    int x, y;
} grid_pos_t;

struct grid
{
    int grid[GRID_SIZE_X][GRID_SIZE_Y];
    int valid_move_positions[GRID_SIZE_X][GRID_SIZE_Y];
    entity_t* entities;
    int mouseGridX, mouseGridY;
    int move_type;
    
    SDL_Texture* tileset;
    int tileset_width;
    int tileset_height;
};

v2 index_to_pos(int index, int xdim)
{
    return vec2(index % xdim, index / xdim);
}

#endif
