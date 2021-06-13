
void
init_cards(SDL_Renderer* renderer) {
    SDL_Surface* temp_surface;
    SDL_Color dark = {25, 25, 25};
    SDL_Color light = {245, 245, 245};
    
    card_textures[0] = IMG_LoadTexture(renderer, "assets/Card-Free.png");
    card_textures[1] = IMG_LoadTexture(renderer, "assets/Card-Horizontal.png");
    card_textures[2] = IMG_LoadTexture(renderer, "assets/Card-Vertical.png");
    card_textures[3] = IMG_LoadTexture(renderer, "assets/Card-Laser.png");
    card_textures[4] = IMG_LoadTexture(renderer, "assets/Card-Cannon.png");
    card_textures[5] = IMG_LoadTexture(renderer, "assets/Card-Blast.png");
    
    
    temp_surface = TTF_RenderText_Solid(main_font, "Free", dark); 
    title_text_textures[0] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    SDL_FreeSurface(temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Horizontal", dark); 
    title_text_textures[1] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    SDL_FreeSurface(temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Vertical", dark); 
    title_text_textures[2] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    SDL_FreeSurface(temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Laser", dark); 
    title_text_textures[3] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    SDL_FreeSurface(temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Cannon", dark); 
    title_text_textures[4] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    SDL_FreeSurface(temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Blast", dark); 
    title_text_textures[5] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    SDL_FreeSurface(temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Your turn, discard cards you don't want.", light); 
    discard_card_texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
    SDL_FreeSurface(temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Your turn, select one new card.", light); 
    select_one_card_texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
    SDL_FreeSurface(temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Your turn, you have too many cards.", light);
    too_many_cards_texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
    SDL_FreeSurface(temp_surface);
    
    temp_surface = TTF_RenderText_Solid(main_font, "Done", light);
    done_texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
    SDL_FreeSurface(temp_surface);
    
    char temp_str[2];
    temp_str[1] = '\0';
    for (int i = 0; i < 10; i++) {
        temp_str[0] = (char) (i + 48);
        temp_surface = TTF_RenderText_Solid(main_font, temp_str, dark); 
        number_textures[i] = SDL_CreateTextureFromSurface(renderer, temp_surface);
    }
}

f32
random_f32() {
    return ((f32) rand() / (f32) RAND_MAX);
}

void
init_random_card_stats(Card* card) {
    card->range = 0;
    if (card->type == CardType_Movement_Free ||
        card->type == CardType_Attack_Cannon) {
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
init_random_card(Card* card) {
    card->type = (s32) (random_f32() * CardType_Count);
    init_random_card_stats(card);
}

void
init_random_movement_card(Card* card) {
    card->type = (s32) (random_f32() * 3);
    init_random_card_stats(card);
}

void
init_random_attack_card(Card* card) {
    card->type = (s32) (random_f32() * 3) + 3;
    init_random_card_stats(card);
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
    
    const int digit_size = 12;
    int num_digits = (s32) log10(number);
    
    SDL_Rect rect;
    rect.x = x + digit_size * num_digits;
    rect.y = y;
    rect.w = digit_size;
    rect.h = FONT_SIZE;
    
    for (;;) {
        int digit = number % 10;
        number /= 10;
        SDL_RenderCopy(renderer, number_textures[digit], 0, &rect);
        rect.x -= digit_size;
        
        if (number == 0) {
            break;
        }
    }
}

void
draw_card(SDL_Renderer* renderer, Card* card, struct grid* grid, v2 pos, f64 angle) { 
    SDL_Rect base;
    base.x = (s32) pos.x;
    base.y = (s32) pos.y;
    base.w = CARD_WIDTH;
    base.h = CARD_HEIGHT;
    
    // Render the card
    if (card->type >= CardType_Movement_First &&
        card->type <= CardType_Count) {
        SDL_Point center;
        center.x = CARD_WIDTH / 2;
        center.y = CARD_HEIGHT;
        SDL_RenderCopyEx(renderer,
                         card_textures[card->type], 
                         0, 
                         &base,
                         angle,
                         &center,
                         SDL_FLIP_NONE);
    }
    
    base.x += 12;
    base.y += 10;
    base.w -= 24;
    base.h -= 18;
    
    if (card->type >= 0 && card->type < (int) array_count(title_text_textures)) {
        s32 text_width = card_type_string_len[card->type] * 12;
        if (text_width > base.w) text_width = base.w;
        SDL_Rect rect;
        rect.x = base.x;
        rect.y = base.y;
        rect.w = text_width;
        
        rect.h = FONT_SIZE;
        SDL_RenderCopy(renderer, title_text_textures[card->type], 0, &rect);
    }
    
    base.y += 40;
    base.h -= 40;
    if (card->range > 0) {
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
        base.x += 54;
        base.w -= 54;
    }
    
    if (card->type >= CardType_Attack_First) {
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
        state.next_state = GameState_Turn;
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
    
    // Select new cards
    if (state.type == GameState_Select_Cards) {
        state.next_state = GameState_Turn;
        if (!player->is_new_cards_initialized) {
            init_random_movement_card(&player->new_cards[0]);
            init_random_attack_card(&player->new_cards[1]);
            player->is_new_cards_initialized = true;
        }
        
        b32 is_pressed = button_was_pressed(&input->mouse_buttons[SDL_BUTTON_LEFT]);
        if (player->num_cards < array_count(player->cards)) {
            v2 p1 = vec2((f32) (WINDOW_WIDTH / 2 - CARD_WIDTH - 32), 128.0f);
            v2 p2 = vec2((f32) (WINDOW_WIDTH / 2 + 32), 128.0f);
            SDL_Rect rect;
            rect.x = (s32) p1.x;
            rect.y = (s32) p1.y;
            rect.w = CARD_WIDTH;
            rect.h = CARD_HEIGHT;
            bool selected_new_card = false;
            if (is_mouse_within_rect(&rect, input) && is_pressed) {
                player->cards[player->num_cards++] = player->new_cards[0];
                selected_new_card = true;
            }
            
            rect.x = (s32) p2.x;
            rect.y = (s32) p2.y;
            rect.w = CARD_WIDTH;
            rect.h = CARD_HEIGHT;
            if (is_mouse_within_rect(&rect, input) && is_pressed) {
                player->cards[player->num_cards++] = player->new_cards[1];
                selected_new_card = true;
            }
            
            if (selected_new_card) {
                state.type = state.next_state;
                state.next_state = GameState_Animation;
                player->is_new_cards_initialized = false;
            }
        } else {
            // Done button
            int text_width = 50;
            SDL_Rect rect;
            rect.x = (WINDOW_WIDTH - text_width)/2;
            rect.y = 92;
            rect.w = text_width;
            rect.h = FONT_SIZE + 8;
            
            if (is_mouse_within_rect(&rect, input) && is_pressed) {
                state.type = state.next_state;
                state.next_state = GameState_Animation;
                player->is_new_cards_initialized = false;
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
    
    if (state.type == GameState_Select_Cards) {
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
        if (player->num_cards < array_count(player->cards)) {
            SDL_RenderCopy(renderer, select_one_card_texture, 0, &rect);
            
            if (player->is_new_cards_initialized) {
                v2 p1 = vec2((f32) (WINDOW_WIDTH / 2 - CARD_WIDTH - 32), 128.0f);
                v2 p2 = vec2((f32) (WINDOW_WIDTH / 2 + 32), 128.0f);
                draw_card(renderer, &player->new_cards[0], grid, p1, 0.0);
                draw_card(renderer, &player->new_cards[1], grid, p2, 0.0);
                
                rect.x = (s32) p1.x;
                rect.y = (s32) p1.y;
                rect.w = CARD_WIDTH;
                rect.h = CARD_HEIGHT;
                if (is_mouse_within_rect(&rect, input)) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderDrawRect(renderer, &rect);
                }
                
                rect.x = (s32) p2.x;
                rect.y = (s32) p2.y;
                rect.w = CARD_WIDTH;
                rect.h = CARD_HEIGHT;
                if (is_mouse_within_rect(&rect, input)) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderDrawRect(renderer, &rect);
                }
            }
        } else {
            SDL_RenderCopy(renderer, select_one_card_texture, 0, &rect);
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
    }
    
    // NOTE(alexander): draw cards
    f32 angle = -60.0f/8.0f;
    f32 angle_increment = (60.0f/4.0f) / (f32) (player->num_cards - 1);
    for (int card_index = 0; card_index < player->num_cards; card_index++) {
        if (player->is_selected && card_index == player->selected_card) {
            continue;
        }
        Card* card = &player->cards[card_index];
        draw_card(renderer, card, grid, player->card_pos[card_index], 0.0);
        angle += angle_increment;
    }
    
    if (player->is_selected) {
        Card* card = &player->cards[player->selected_card];
        draw_card(renderer, card, grid, player->card_pos[player->selected_card], 0.0);
        angle += angle_increment;
    }
}