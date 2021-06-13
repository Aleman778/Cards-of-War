
#define CARD_WIDTH 120
#define CARD_HEIGHT 200

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

static const s32 card_type_string_len[6] = {
    4, 9, 8, 5, 6, 5
};

static const s32 card_max_attack[6] = {
    0, 0, 0, 3, 5, 7
};

static const s32 card_max_free_movement = 7;
static const s32 card_max_blast_radius = 4;

typedef s32 Card_Type;

typedef struct {
    Card_Type type;
    s32 range; // 0 means unlimited range
    s32 attack;
} Card;

typedef struct {
    Card cards[16];
    v2 card_pos[16];
    bool discard_cards[16];
    s32 num_cards;
    s32 selected_card;
    b32 is_selected;
} Player_Hand;

static SDL_Texture* title_text_textures[CardType_Count];
static SDL_Texture* symbol_textures[CardType_Count];
static SDL_Texture* number_textures[10];
static SDL_Texture* discard_card_texture;
static SDL_Texture* select_one_card_texture;
static SDL_Texture* done_texture;


bool
is_mouse_within_rect(SDL_Rect* rect, Input* input) {
    s32 mx = (s32) input->mouse.x;
    s32 my = (s32) input->mouse.y;
    return (mx > rect->x &&
            my > rect->y &&
            mx < rect->x + rect->w &&
            my < rect->y + rect->h);
}
