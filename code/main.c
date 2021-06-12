#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

static TTF_Font* main_font = 0; // NOTE(alexander): this is global for now

#include "types.h"
#include "vecmath.h"
#include "player_grid/player.h"
#include "player_grid/grid.h"
#include "cards.c"

static bool is_running = false;

int main(int argc, char* argv[]) {
    srand(time(0));
    
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
            
            // Simple vector math test
            v2 test_vec = vec2(10.0f, 15.0f);
            test_vec = vec2_adds(test_vec, 3.0f);
            test_vec = vec2_add(test_vec, vec2_left);
            printf("%f, %f\n", test_vec.x, test_vec.y);
            
            grid_t grid;
            grid_init(&grid);

            grid_random_fill(&grid);
            
            Card card;
            card.type = CardType_Movement_Horizontal;
            card.range = 0;
            card.attack = 0;
            
            player_t player;
            player_init(&player);
            player.posX = GRID_SIZE_X / 2;
            player.posY = GRID_SIZE_Y / 2;

            grid.player = &player;
            grid.grid[player.posX][player.posY] = GRID_NONE;
            memset(grid.valid_move_positions, 0, sizeof(grid.valid_move_positions));
            grid_compute_reachable_positions(&grid, player.posX, player.posY, PLAYER_MOVE_DISTANCE);
            
            // Main game loop
            is_running = true;
            while (is_running) {
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        is_running = false;
                    }
                    else if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN)
                    {
                        int x, y;
                        SDL_GetMouseState(&x, &y);
                        if (x < GRID_ELEM_WIDTH * GRID_SIZE_X && y < GRID_ELEM_HEIGHT * GRID_SIZE_Y)
                        {
                            grid_onmouseevent(&grid, x, y, event.type);
                        }
                    }
                    else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_a)
                    {
                        grid.move_type = (grid.move_type + 1) % MOVE_TYPE_MAX; // Temporary way of switching movement types, should be done through cards later
                    }
                }
                // Rendering
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, 0, &dest);
                
                grid_render(renderer, &grid);
                
                // Render players hand of cards
                draw_card(renderer, &card, 300, 300);
                
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