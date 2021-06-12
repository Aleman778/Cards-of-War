#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#include "types.h"
#include "vecmath.h"
#include "player_grid/player.h"
#include "player_grid/grid.h"

static TTF_Font* main_font = 0; // NOTE(alexander): this is global for now
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
            SDL_Color White = {255, 255, 255};
            
            main_font = TTF_OpenFont("assets/fonts/dpcomic.ttf", 16);
            printf("%p\n", main_font);
            SDL_Surface* surfaceMessage = TTF_RenderText_Solid(main_font, "put your text here", White); 
            
            // now you can convert it into a texture
            SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
            
            SDL_Rect Message_rect; //create a rect
            Message_rect.x = 0;  //controls the rect's x coordinate 
            Message_rect.y = 0; // controls the rect's y coordinte
            Message_rect.w = 300; // controls the width of the rect
            Message_rect.h = 30; // controls the height of the rect
            
            // (0,0) is on the top left of the window/screen,
            // think a rect as the text's box,
            // that way it would be very simple to understand
            
            // Now since it's a texture, you have to put RenderCopy
            // in your game loop area, the area where the whole code executes
            
            // you put the renderer's name first, the Message,
            // the crop size (you can ignore this if you don't want
            // to dabble with cropping), and the rect which is the size
            // and coordinate of your texture
            //SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
            
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
                
                //SDL_RenderCopy(renderer, texture, 0, &dest);
                SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
                
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