#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define FONT_SIZE 16

#define MAX_ENTITIES 128

static TTF_Font* main_font = 0; // NOTE(alexander): this is global for now

#include "types.h"
#include "vecmath.h"
#include "cards.h"
#include "grid.h"
#include "entity.h"
#include "input.h"
#include "game_state.h"

#include "grid.c"
#include "cards.c"
#include "map.c"
#include "entity.c"

static bool is_running = false;
static Game_State state;

int main(int argc, char* argv[]) {
    (void) argc; // Unused variable, this statement can be removed when the variable has been used elsewhere
    (void) argv; // Unused variable, this statement can be removed when the variable has been used elsewhere
    (void) symbol_textures; // Unused variable, this statement can be removed when the variable has been used elsewhere
    
    srand((int) time(0));
    
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) >= 0) {
        
        if(TTF_Init() == -1) {
            printf("TTF_Init: %s\n", TTF_GetError());
            return -1;
        }
        
        SDL_Window* window = SDL_CreateWindow("GMTK Game Jam 2021",
                                              SDL_WINDOWPOS_UNDEFINED,
                                              SDL_WINDOWPOS_UNDEFINED,
                                              WINDOW_WIDTH, WINDOW_HEIGHT,
                                              SDL_WINDOW_SHOWN);
        if (window) {
            SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
                                                        SDL_RENDERER_PRESENTVSYNC |
                                                        SDL_RENDERER_ACCELERATED);
            // Load font
            main_font = TTF_OpenFont("assets/fonts/dpcomic.ttf", FONT_SIZE);
            init_cards(renderer);
            
            struct grid grid;
            grid_init(&grid);
            
            // Player Hand
            Player_Hand player_hand;
            zero_struct(player_hand);
            
            // Add some random cards
            for (int i = 0; i < 8; i++) {
                init_random_card(&player_hand.cards[i]);
            } 
            player_hand.num_cards = 8;
            
            entity_t entities[MAX_ENTITIES];
            zero_struct(entities);
            int num_entities = 0;
            grid.entities = entities;
            
            
            entity_t* player_1 = entity_init(&entities[num_entities++]);
            player_1->posX = 1;
            player_1->posY = 1;
            player_1->playerControlled = true;
            
            entity_t* enemy_1 = entity_init(&entities[num_entities++]);
            enemy_1->posX = 12;
            enemy_1->posY = 1;
            
            // Setup game state
            state.type = GameState_Discard_Cards;
            state.next_state = GameState_Turn;
            state.current_entity = player_1;
            
            // Load tilemap
            read_tmx_map_data("assets/GMTK-1.tmx", grid.grid);
            grid.tileset = IMG_LoadTexture(renderer, "assets/grass_tileset.png");
            SDL_QueryTexture(grid.tileset, NULL, NULL, &grid.tileset_width, &grid.tileset_width);
            
            // Setup input
            Input input;
            input.mouse = vec2(0.0f, 0.0f);
            
            // Main game loop
            is_running = true;
            while (is_running) {
                // Reset input states
                zero_struct(input.mouse_buttons);
                
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_QUIT: {
                            is_running = false;
                        } break;
                        
                        case SDL_MOUSEMOTION: {
                            int x, y;
                            SDL_GetMouseState(&x, &y);
                            input.mouse.x = (f32) x;
                            input.mouse.y = (f32) y;
                        } break;
                        
                        case SDL_MOUSEBUTTONDOWN:
                        case SDL_MOUSEBUTTONUP: {
                            SDL_MouseButtonEvent mouse = event.button;
                            if ((u32) mouse.button < array_count(input.mouse_buttons)) {
                                input.mouse_buttons[mouse.button].num_half_transitions++;
                                input.mouse_buttons[mouse.button].ended_down =
                                    event.type == SDL_MOUSEBUTTONDOWN;
                            }
                        } break;
                        
                        case SDL_KEYDOWN: {
                            // TODO(alexander): do we need this?
                        } break;
                    } 
                }
                // Update player hand
                update_player_hand(&player_hand, &input, &grid, player_1);
                
                // Move player
                if (grid_perform_action(&grid, &input, &player_hand, player_1)) {
                    
                }
                
                // Rendering
                SDL_RenderClear(renderer);
                
                grid_render(renderer, &grid);
                
                // Render players hand of cards
                draw_player_cards(renderer, &player_hand);
                
                // Render to the screen
                SDL_RenderPresent(renderer);
            }
            
            // Close the window
            SDL_DestroyWindow(window);
        } else {
            printf("Failed to create the window: %s\n", SDL_GetError());
        }
        
        SDL_Quit();
    } else {
        printf("Could not initialize SDL2\n.");
    }
    
    return 0;
}