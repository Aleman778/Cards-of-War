
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
        card->range = (s32) (random_f32() * (card_max_free_movement - 2));
        card->range += 2;
    } else if (card->type == CardType_Attack_Blast) {
        card->range = (s32) (random_f32() * card_max_blast_radius);
        card->range += 2;
    } else {
        card->range = 0;
    }
    
    if (card->type >= CardType_Attack_First) {
        card->attack = (s32) (random_f32() * (card_max_attack[card->type] - 1));
        card->attack++;
    } else {
        card->attack = 0;
    }
}

void
draw_number(SDL_Renderer* renderer, int number, int x, int y) {
    assert(number >= 0);
    
    int num_digits = (s32) log10(number);
    
    SDL_Rect rect;
    rect.x = x + FONT_SIZE * num_digits;
    rect.y = y;
    rect.w = FONT_SIZE;
    rect.h = FONT_SIZE;
    
    for (;;) {
        int digit = number % 10;
        number /= 10;
        SDL_RenderCopy(renderer, number_textures[digit], 0, &rect);
        rect.x -= FONT_SIZE;
        
        if (number == 0) {
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
    
    if (card->type >= 0 && card->type < (int) array_count(title_text_textures)) {
        s32 text_width = card_type_string_len[card->type] * FONT_SIZE;
        if (text_width > base.w) text_width = base.w;
        SDL_Rect rect;
        rect.x = base.x;
        rect.y = base.y;
        rect.w = text_width;
        
        rect.h = FONT_SIZE;
        SDL_RenderCopy(renderer, title_text_textures[card->type], 0, &rect);
    }
    
    if (card->range > 0) {
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
update_player_hand(Player_Hand* player, Input* input, struct grid* grid, entity_t* entity) {
    int num_cards = player->num_cards;
    if (player->is_selected) {
        num_cards--;
    }
    int x_offset = (WINDOW_WIDTH - CARD_WIDTH * num_cards) / 2;
    int card_spacing = CARD_WIDTH;
    if (x_offset < 0) {
        card_spacing = WINDOW_WIDTH / num_cards;
        x_offset = 0;
    }
    
    int card_space_index = 0;
    for (int card_index = 0; card_index < num_cards; card_index++) {
        Card* card = &player->cards[card_index];
        v2 p = vec2((f32) (x_offset + card_spacing*card_space_index), 400.0f);
        bool hover_card = input->mouse.x > p.x &&
            input->mouse.y > p.y &&
            input->mouse.x < p.x + card_spacing &&
            input->mouse.y < p.y + card_spacing; 
        if (!player->is_selected && hover_card && 
            button_is_down(&input->mouse_buttons[SDL_BUTTON_LEFT])) {
            
            player->selected_card = card_index;
            player->is_selected = true;
            
            int move_distance = card->range;
            if (move_distance == 0) {
                move_distance = 100;
            }
            
            entity->selected = true;
            memset(grid->valid_move_positions, 0, sizeof(grid->valid_move_positions));
            grid_compute_reachable_positions(grid, entity->posX, entity->posY, 
                                             card->range);
            grid->card = card;
        }
        
        if (hover_card && !player->is_selected) {
            p.y = WINDOW_HEIGHT - CARD_HEIGHT;
            if (p.x < 0.0f) p.x = 0.0f;
            if (p.x > WINDOW_WIDTH - CARD_WIDTH) p.x = WINDOW_WIDTH - CARD_WIDTH;
        }
        
        if (player->is_selected && player->selected_card == card_index) {
            p = input->mouse;
        }
        
        player->card_pos[card_index] = p;
        if (!player->is_selected || player->selected_card != card_index) {
            card_space_index++;
        }
    }
}

void
draw_player_cards(SDL_Renderer* renderer, Player_Hand* player) {
    for (int card_index = 0; card_index < player->num_cards; card_index++) {
        Card* card = &player->cards[card_index];
        draw_card(renderer, card, player->card_pos[card_index]);
    }
}