#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#include "types.h"
#include "vecmath.h"
#include "player_grid/player.h"
#include "player_grid/grid.h"

static bool is_running = false;

int main(int argc, char* argv[]) {
    srand(time(0));
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) >= 0) {
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
            
            // Simple vector math test
            v2 test_vec = vec2(10.0f, 15.0f);
            test_vec = vec2_adds(test_vec, 3.0f);
            test_vec = vec2_add(test_vec, vec2_left);
            printf("%f, %f\n", test_vec.x, test_vec.y);

            grid_t grid;
            grid_init(&grid);

            player_t player;
            player_init(&player);
            player.posX = GRID_SIZE_X / 2;
            player.posY = GRID_SIZE_Y / 2;
            
            // Main game loop
            is_running = true;
            while (is_running) {
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        is_running = false;
                    }
                }
                // Rendering
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, 0, &dest);

                grid_render(renderer, &grid, &player);
                
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