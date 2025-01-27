#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include "render.h"
#include "structs.h"
#include "config.h"
#include "simulation.h"

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Liquid Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    
    draw_grid(surface);

    struct Cell environment[ROWS*COLUMNS];
    initialize_environment(environment);

    //Event loop
    int simulation_running = 1;
    SDL_Event event;
    int current_type = SOLID_TYPE;
    int delete_mode = 0;

    while(simulation_running) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                simulation_running = 0;
            }

            if(event.type == SDL_MOUSEMOTION) {
                if(event.motion.state != 0) { //if a button is pressed
                    int cell_x = event.motion.x / CELL_SIZE; //by dividing it results in the starting x, y position to draw the cell in our grid
                    int cell_y = event.motion.y / CELL_SIZE;
                    struct Cell cell;

                    if(delete_mode != 0) {
                        current_type = WATER_TYPE;
                        cell = (struct Cell) { current_type, 0.0, cell_x, cell_y };    
                    }
                    else {
                        cell = (struct Cell) { current_type, environment[cell_x+COLUMNS*cell_y].fill_level + 1, cell_x, cell_y }; //in ep.4 it is +0.2 insteado +1
                    }

                    environment[cell_x + COLUMNS * cell_y] = cell;
                }
            }

            if(event.type == SDL_KEYDOWN) {
                if(event.key.keysym.sym == SDLK_SPACE) {
                    current_type = !current_type;
                }

                if(event.key.keysym.sym == SDLK_BACKSPACE) {
                    delete_mode = !delete_mode;
                }
            }
        }

        simulation_step(environment);
        draw_environment(surface, environment);
        draw_grid(surface);
        SDL_UpdateWindowSurface(window);

        SDL_Delay(30);
    }

    return 0;
}