#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define MAX_ENTITIES 128

static TTF_Font* main_font = 0; // NOTE(alexander): this is global for now

#include "types.h"
#include "vecmath.h"
#include "cards.h"
#include "grid.h"
#include "entity.h"
#include "input.h"

#include "cards.c"
#include "grid.c"

static bool is_running = false;

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
            
            SDL_Texture* texture = IMG_LoadTexture(renderer, "assets/grass_tileset.bmp");
            SDL_Rect dest;
            dest.x = 0;
            dest.y = 0;
            dest.w = WINDOW_WIDTH;
            dest.h = WINDOW_HEIGHT;
            
            // Load font
            main_font = TTF_OpenFont("assets/fonts/dpcomic.ttf", 16);
            init_cards(renderer);
            
            grid_t grid;
            grid_init(&grid);
            
            grid_random_fill(&grid);
            
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
            player_1->posX = GRID_SIZE_X / 2;
            player_1->posY = GRID_SIZE_Y / 2;
            grid.grid[player_1->posX][player_1->posY] = GRID_NONE;

            entity_t* player_2 = entity_init(&entities[num_entities++]);
            player_2->posX = GRID_SIZE_X / 4;
            player_2->posY = GRID_SIZE_Y / 4;
            grid.grid[player_2->posX][player_2->posY] = GRID_NONE;
            
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
                            grid.move_type = (grid.move_type + 1) % MOVE_TYPE_MAX;
                            // TODO(alexander): do we need this?
                        } break;
                    } 
                }
                
                // Update player hand
                update_player_hand(&player_hand);
                
                // Move player
                if (input.mouse.x < GRID_ELEM_WIDTH * GRID_SIZE_X && input.mouse.y < GRID_ELEM_HEIGHT * GRID_SIZE_Y)
                {
                    grid_move_player(&grid, &input);
                }
                
                // Rendering
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, 0, &dest);
                
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