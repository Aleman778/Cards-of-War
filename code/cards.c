
 enum {
    CardType_Movement_Free = 0,
    CardType_Movement_Horizontal = 1,
    CardType_Movement_Vertical = 2,
    CardType_Attack_Laser = 3,
    CardType_Attack_Cannon = 4,
    CardType_Attack_Blast = 5,
};
typedef s32 Card_Type;

typedef struct {
    Card_Type type;
    s32 range; // 0 means unlimited range
    s32 attack;
} Card;

static SDL_Texture* title_text_textures[6];
static SDL_Texture* symbol_textures[6];
static SDL_Texture* number_textures[10];

void
init_cards(SDL_Renderer* renderer) {
    SDL_Surface* temp_surface;
    SDL_Color color = {25, 25, 25};
    
    temp_surface = TTF_RenderText_Solid(main_font, "Free", color); 
    title_text_textures[0] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Horizontal", color); 
    title_text_textures[1] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Vertical", color); 
    title_text_textures[2] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Laser", color); 
    title_text_textures[3] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Cannon", color); 
    title_text_textures[4] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Blast", color); 
    title_text_textures[5] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    
    char temp_str[2];
    temp_str[1] = '\0';
    for (int i = 0; i < 10; i++) {
        temp_str[0] = i + 48;
        temp_surface = TTF_RenderText_Solid(main_font, temp_str, color); 
        number_textures[i] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    }
    
    SDL_FreeSurface(temp_surface);
}

void
draw_number(SDL_Renderer* renderer, int number, int x, int y) {
    assert(number >= 0);
    
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = 24;
    rect.h = 24;
    
    for (;;) {
        int digit = number % 10;
        number /= 10;
        SDL_RenderCopy(renderer, number_textures[digit], 0, &rect);
        
        if (number <= 9) {
            break;
        }
    }
}

void
draw_card(SDL_Renderer* renderer, Card* card, int x, int y) { 
    SDL_Rect base;
    base.x = x;
    base.y = y;
    base.w = 120;
    base.h = 240;
    
    // Render the card
    if (card->type < 3) {
        SDL_SetRenderDrawColor(renderer, 192, 212, 190, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 148, 153, 166, 255);
    }
    SDL_RenderDrawRect(renderer, &base);
    if (card->type < 3) {
        SDL_SetRenderDrawColor(renderer, 192, 212, 190, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 190, 197, 212, 255);
    }
    SDL_RenderFillRect(renderer, &base);
    
    base.x += 8;
    base.y += 8;
    base.w -= 16;
    base.h -= 16;
    
    if (card->type >= 0 && card->type < array_count(title_text_textures)) {
        SDL_Rect rect = {base.x, base.y, base.w, 24};
        SDL_RenderCopy(renderer, title_text_textures[card->type], 0, &rect);
    }
    
    if (card->type < 3) {
        base.y += 40;
        base.h -= 40;
        draw_number(renderer, card->range, base.x, base.y);
    }
    
    if (card->type >= 3) {
        base.y += 40;
        base.h -= 40;
        draw_number(renderer, card->attack, base.x, base.y);
    }
}