
#define GRID_ELEM_WIDTH 32
#define GRID_ELEM_HEIGHT GRID_ELEM_WIDTH

#define GRID_SIZE_X (WINDOW_WIDTH / GRID_ELEM_WIDTH)
#define GRID_SIZE_Y ((WINDOW_HEIGHT - (GRID_ELEM_HEIGHT * 5)) / GRID_ELEM_HEIGHT)

#define PLAYER_MOVE_DISTANCE 4

enum grid_objects
{
    GRID_NONE,
    //GRID_PLAYER,
    GRID_OBSTACLE,
};

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

typedef struct
{
    int grid[GRID_SIZE_X][GRID_SIZE_Y];
    int valid_move_positions[GRID_SIZE_X][GRID_SIZE_Y];
    player_t* player;
    int mouseGridX, mouseGridY;
    int move_type;
} grid_t;


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

bool grid_pos_within_player_range(grid_t* grid, int gridX, int gridY)
{
    if (gridX >= GRID_SIZE_X || gridY >= GRID_SIZE_Y)
        return false;
    
    switch (grid->move_type)
    {
        case MOVE_TYPE_IGNORE_OBSTACLES: {
            return abs(grid->player->posX - gridX) + abs(grid->player->posY - gridY) <= PLAYER_MOVE_DISTANCE && grid->grid[gridX][gridY] == GRID_NONE;
        } break;
        case MOVE_TYPE_HORIZONTAL: {
            if (grid->player->posY != gridY)
                return false;
            
            for (int x = grid->player->posX; x != gridX; x += signum(gridX - grid->player->posX))
            {
                if (grid->grid[x][gridY] != GRID_NONE)
                    return false;
            }
            return grid->grid[gridX][gridY] == GRID_NONE;
        } break;
        case MOVE_TYPE_VERTICAL: {
            if (grid->player->posX != gridX)
                return false;
            for (int y = grid->player->posY; y != gridY; y += signum(gridY - grid->player->posY))
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

void grid_move_player(grid_t* grid, Input* input)
{
    grid->mouseGridX = input->mouse_x / GRID_ELEM_WIDTH;
    grid->mouseGridY = input->mouse_y / GRID_ELEM_HEIGHT;
    if (grid->player)
    {
        grid->player->underMouseCursor = false;
        if (input->mouse_x > grid->player->posX * GRID_ELEM_WIDTH && input->mouse_x < grid->player->posX * GRID_ELEM_WIDTH + GRID_ELEM_WIDTH)
        {
            if (input->mouse_y > grid->player->posY * GRID_ELEM_HEIGHT && input->mouse_y < grid->player->posY * GRID_ELEM_HEIGHT + GRID_ELEM_HEIGHT)
            {
                grid->player->underMouseCursor = true;
            }
        }
        
        if (button_was_pressed(&input->mouse_buttons[SDL_BUTTON_LEFT]))
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
                    memset(grid->valid_move_positions, 0, sizeof(grid->valid_move_positions));
                    grid_compute_reachable_positions(grid, grid->player->posX, grid->player->posY, PLAYER_MOVE_DISTANCE);
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
