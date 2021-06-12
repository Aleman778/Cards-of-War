
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