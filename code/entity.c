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
}