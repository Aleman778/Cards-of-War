
typedef enum {
    CardType_Movement_Free,
    CardType_Movement_Horizontal,
    CardType_Movement_Vertical,
    CardType_Attack_Laser,
    CardType_Attack_Ranged_Cannon,
    CardType_Attack_Ranged,
} Card_Type;

typedef struct {
    Card_Type type;
    s32 range; // 0 means unlimited range
    s32 damage;
    // NOTE(alexander): TODO texture
} Card;


void
draw_card(SDL_Renderer* renderer, Card* card) { 
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 40;
    rect.h = 60;
    
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderDrawRect(renderer, &rect);
    
    
    
}