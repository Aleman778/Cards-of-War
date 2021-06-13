
void
init_cards(SDL_Renderer* renderer) {
    SDL_Surface* temp_surface;
    SDL_Color dark = {25, 25, 25};
    SDL_Color light = {245, 245, 245};
    
    temp_surface = TTF_RenderText_Solid(main_font, "Free", dark); 
    title_text_textures[0] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Horizontal", dark); 
    title_text_textures[1] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Vertical", dark); 
    title_text_textures[2] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Laser", dark); 
    title_text_textures[3] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Cannon", dark); 
    title_text_textures[4] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Blast", dark); 
    title_text_textures[5] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Your turn, discard cards you don't want.", light); 
    discard_card_texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Your turn, select one new card.", light); 
    select_one_card_texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Done", light);
    done_texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
    
    
    char temp_str[2];
    temp_str[1] = '\0';
    for (int i = 0; i < 10; i++) {
        temp_str[0] = (char) (i + 48);
        temp_surface = TTF_RenderText_Solid(main_font, temp_str, dark); 
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
init_player_hand(Player_Hand* player) {
    zero_struct(*player);
    
    // Add some random cards
    for (int i = 0; i < 8; i++) {
        init_random_card(&player->cards[i]);
    } 
    player->num_cards = 8;
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
draw_card(SDL_Renderer* renderer, Card* card, struct grid* grid, v2 pos) { 
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
        SDL_Rect tr;
        tr.x = 16;
        tr.y = 96;
        tr.w = 16;
        tr.h = 16;
        SDL_Rect dst = base;
        dst.w = 16;
        dst.h = 16;
        SDL_RenderCopy(renderer, grid->tileset, &tr, &dst);
        draw_number(renderer, card->range, base.x + 20, base.y);
    }
    
    if (card->type >= CardType_Attack_First) {
        base.y += 40;
        base.h -= 40;
        SDL_Rect tr;
        tr.x = 0;
        tr.y = 96;
        tr.w = 16;
        tr.h = 16;
        SDL_Rect dst = base;
        dst.w = 16;
        dst.h = 16;
        SDL_RenderCopy(renderer, grid->tileset, &tr, &dst);
        draw_number(renderer, card->attack, base.x + 20, base.y);
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
    for (int card_index = 0; card_index < player->num_cards; card_index++) {
        Card* card = &player->cards[card_index];
        v2 p = vec2((f32) (x_offset + card_spacing*card_space_index), 400.0f);
        bool hover_card = input->mouse.x > p.x &&
            input->mouse.y > p.y &&
            input->mouse.x < p.x + card_spacing &&
            input->mouse.y < p.y + card_spacing; 
        
        if (state.type == GameState_Turn && !player->is_selected && hover_card && 
            button_is_down(&input->mouse_buttons[SDL_BUTTON_LEFT])) {
            
            player->selected_card = card_index;
            player->is_selected = true;
            
            int move_distance = card->range;
            if (move_distance == 0) {
                move_distance = 100;
            }
            
            entity->selected = true;
            memset(entity->valid_move_positions, 0, GRID_SIZE_BYTES);
            grid_compute_reachable_positions(grid, entity, entity->posX, entity->posY,
                                             card->range);
            grid->card = card;
            entity->lastCard = *card;
        }
        
        bool discard = player->discard_cards[card_index];
        if (state.type == GameState_Discard_Cards &&
            button_was_pressed(&input->mouse_buttons[SDL_BUTTON_LEFT]) && hover_card) {
            discard = !discard;
            player->discard_cards[card_index] = discard;
            
        }
        
        if (discard) p.y = WINDOW_HEIGHT - 32;
        
        if (hover_card && !player->is_selected && !discard) {
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
    
    // Discard cards
    if (state.type == GameState_Discard_Cards) {
        if (button_was_pressed(&input->mouse_buttons[SDL_BUTTON_LEFT])) {
            // Done button
            int text_width = 50;
            SDL_Rect rect;
            rect.x = (WINDOW_WIDTH - text_width)/2;
            rect.y = 92;
            rect.w = text_width;
            rect.h = FONT_SIZE + 8;
            
            if (is_mouse_within_rect(&rect, input)) {
                // Performs discard and hands over to next state 
                state.type = state.next_state;
                state.next_state = GameState_Animation;
                for (int card_index = 0; card_index < player->num_cards; card_index++) {
                    if (player->discard_cards[card_index]) {
                        player->discard_cards[card_index] = false;
                        int cards_right = player->num_cards - player->selected_card;
                        if (cards_right > 0) {
                            memmove(player->cards + card_index, 
                                    player->cards + (card_index + 1),
                                    cards_right * sizeof(Card));
                            memmove(player->discard_cards + card_index, 
                                    player->discard_cards + (card_index + 1),
                                    cards_right);
                        } 
                        
                        player->num_cards--;
                        card_index--;
                    }
                }
                
                // Create new cards
                for (int i = player->num_cards; i < 8; i++) {
                    init_random_card(&player->cards[i]);
                } 
                player->num_cards = 8;
            }
        }
    }
}

void
draw_player_cards(SDL_Renderer* renderer, Player_Hand* player, struct grid* grid, Input* input) {
    // NOTE(alexander): draw message
    if (state.type == GameState_Discard_Cards) {
        // Render message
        int text_width = 360;
        SDL_Rect rect;
        rect.x = (WINDOW_WIDTH - text_width)/2;
        rect.y = 64;
        rect.w = text_width;
        rect.h = FONT_SIZE + 8;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
        rect.x += 4;
        rect.y += 4;
        rect.h -= 8;
        rect.w -= 8;
        SDL_RenderCopy(renderer, discard_card_texture, 0, &rect);
        
        // Done button
        text_width = 50;
        rect.x = (WINDOW_WIDTH - text_width)/2;
        rect.y = 92;
        rect.w = text_width;
        rect.h = FONT_SIZE + 8;
        
        if (is_mouse_within_rect(&rect, input)) {
            if (button_is_down(&input->mouse_buttons[SDL_BUTTON_LEFT])) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 125, 255);
            }
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        }
        
        SDL_RenderFillRect(renderer, &rect);
        rect.x += 4;
        rect.y += 4;
        rect.h -= 8;
        rect.w -= 8;
        SDL_RenderCopy(renderer, done_texture, 0, &rect);
        
    }
    
    // NOTE(alexander): draw cards
    for (int card_index = 0; card_index < player->num_cards; card_index++) {
        Card* card = &player->cards[card_index];
        draw_card(renderer, card, grid, player->card_pos[card_index]);
    }
}