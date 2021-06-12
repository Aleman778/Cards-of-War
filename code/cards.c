
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
        temp_str[0] = (char) (i + 48);
        temp_surface = TTF_RenderText_Solid(main_font, temp_str, color); 
        number_textures[i] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    }
    
    SDL_FreeSurface(temp_surface);
}

f32
random_f32() {
    return ((f32) rand() / (f32) RAND_MAX);
}

void
init_random_card(Card* card) {
    card->type = (s32) (random_f32() * CardType_Count);
    card->range = 0;
    if (card->type == CardType_Movement_Free) {
        card->range = (s32) (random_f32() * card_max_attack[card->type]);
    } else {
        card->range = 0;
    }
    
    if (card->type >= CardType_Attack_First) {
        card->attack = (s32) (random_f32() * card_max_attack[card->type]);
    } else {
        card->attack = 0;
    }
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
        number /= 10;// TODO(alexander): this will be revese order :P, maybe doesn't need > 9
        SDL_RenderCopy(renderer, number_textures[digit], 0, &rect);
        
        if (number <= 9) {
            break;
        }
    }
}



void
draw_card(SDL_Renderer* renderer, Card* card, v2 pos) { 
    SDL_Rect base;
    base.x = (s32) pos.x;
    base.y = (s32) pos.y;
    base.w = CARD_WIDTH;
    base.h = CARD_HEIGHT;
    
    // Render the card
    if (card->type <= CardType_Movement_Last) {
        SDL_SetRenderDrawColor(renderer, 192, 212, 190, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 148, 153, 166, 255);
    }
    SDL_RenderDrawRect(renderer, &base);
    if (card->type <= CardType_Movement_Last) {
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
        SDL_Rect rect;
        rect.x = base.x;
        rect.y = base.y;
        rect.w = base.w;
        rect.h = 24;
        SDL_RenderCopy(renderer, title_text_textures[card->type], 0, &rect);
    }
    
    if (card->type < CardType_Movement_First) {
        base.y += 40;
        base.h -= 40;
        draw_number(renderer, card->range, base.x, base.y);
    }
    
    if (card->type >= CardType_Attack_First) {
        base.y += 40;
        base.h -= 40;
        draw_number(renderer, card->attack, base.x, base.y);
    }
}

void
update_player_hand(Player_Hand* player) {
    int x_offset = (WINDOW_WIDTH - CARD_WIDTH * player->num_cards) / 2;
    for (int card_index = 0; card_index < player->num_cards; card_index++) {
        player->card_pos[card_index] = vec2((f32) (x_offset + CARD_WIDTH*card_index), 400.0f);
    }
}

void
draw_player_cards(SDL_Renderer* renderer, Player_Hand* player) {
    int x_offset = (WINDOW_WIDTH - CARD_WIDTH * player->num_cards) / 2;
    for (int card_index = 0; card_index < player->num_cards; card_index++) {
        Card* card = &player->cards[card_index];
        draw_card(renderer, card, player->card_pos[card_index]);
    }
}