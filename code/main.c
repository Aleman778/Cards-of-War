#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

static TTF_Font* main_font = 0; // NOTE(alexander): this is global for now

#include "types.h"
#include "vecmath.h"
#include "cards.h"
#include "player_grid/player.h"
#include "input.h"

#include "cards.c"
#include "player_grid/grid.c"

static bool is_running = false;

int main(int argc, char* argv[]) {
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
            
            player_t player;
            player_init(&player);
            player.posX = GRID_SIZE_X / 2;
            player.posY = GRID_SIZE_Y / 2;
            
            // Player Hand
            Player_Hand player_hand;
            zero_struct(player_hand);
            
            // Add some random cards
            for (int i = 0; i < 8; i++) {
                init_random_card(&player_hand.cards[i]);
            } 
            player_hand.num_cards = 8;
            
            grid.player = &player;
            grid.grid[player.posX][player.posY] = GRID_NONE;
            memset(grid.valid_move_positions, 0, sizeof(grid.valid_move_positions));
            grid_compute_reachable_positions(&grid, player.posX, player.posY, PLAYER_MOVE_DISTANCE);
            
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