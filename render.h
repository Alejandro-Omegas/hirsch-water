#include <SDL2/SDL.h>
#include "structs.h"
#include "config.h"

#ifndef RENDER_H
#define RENDER_H
Uint32 get_interpolated_color(Uint32 min, Uint32 max, double percentage);
void draw_cell(SDL_Surface* surface, struct Cell cell);
void draw_grid(SDL_Surface* surface);
void initialize_environment(struct Cell environment[ROWS*COLUMNS]);
void draw_environment(SDL_Surface* surface, struct Cell environment[ROWS*COLUMNS]);
#endif