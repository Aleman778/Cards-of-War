
#define CARD_WIDTH 120
#define CARD_HEIGHT 240

enum CardType {
    CardType_Movement_First = 0,
    CardType_Movement_Free = 0,
    CardType_Movement_Horizontal = 1,
    CardType_Movement_Vertical = 2,
    CardType_Movement_Last = 2,
    CardType_Attack_First = 3,
    CardType_Attack_Laser = 3,
    CardType_Attack_Cannon = 4,
    CardType_Attack_Blast = 5,
    CardType_Attack_Last = 6,
    CardType_Count = 6,
};

static const s32 card_max_attack[6] = {
    0, 0, 0, 3, 5, 7
};

typedef s32 Card_Type;

typedef struct {
    Card_Type type;
    s32 range; // 0 means unlimited range
    s32 attack;
} Card;

typedef struct {
    Card cards[16];
    int num_cards;
} Player_Hand;

static SDL_Texture* title_text_textures[CardType_Count];
static SDL_Texture* symbol_textures[CardType_Count];
static SDL_Texture* number_textures[10];