#include <stdlib.h>
#include "render.h"
#include "config.h"

Uint32 get_interpolated_color(Uint32 min, Uint32 max, double percentage) {
 	Uint32 color1 = min;
	Uint32 color2 = max;
	unsigned char   r1 = (color1 >> 16) & 0xff;
    unsigned char   r2 = (color2 >> 16) & 0xff;
    unsigned char   g1 = (color1 >> 8) & 0xff;
    unsigned char   g2 = (color2 >> 8) & 0xff;
    unsigned char   b1 = color1 & 0xff;
    unsigned char   b2 = color2 & 0xff;

    return (int) ((r2 - r1) * percentage + r1) << 16 |
            (int) ((g2 - g1) * percentage + g1) << 8 |
            (int) ((b2 - b1) * percentage + b1);
}

void draw_cell(SDL_Surface* surface, struct Cell cell) {
    int pixel_x = cell.x*CELL_SIZE;
    int pixel_y = cell.y*CELL_SIZE;
    SDL_Rect cell_rect = (SDL_Rect) { pixel_x, pixel_y, CELL_SIZE, CELL_SIZE };

     SDL_FillRect(surface, &cell_rect, COLOR_BLACK); //BG color

    //water fill level
    if(cell.type == WATER_TYPE) {
        int water_height = cell.fill_level > 1 ? CELL_SIZE : cell.fill_level * CELL_SIZE;
        int empty_height = CELL_SIZE - water_height;
        Uint32 interpolated_color = get_interpolated_color(COLOR_BLUE_MIN, COLOR_BLUE_MAX, cell.fill_level);

        SDL_Rect water_rect = (SDL_Rect) { pixel_x, pixel_y + empty_height, CELL_SIZE, water_height }; 
        SDL_FillRect(surface, &water_rect, interpolated_color);
    }

    //solid blocks
    if(cell.type == SOLID_TYPE) {
        SDL_FillRect(surface, &cell_rect, COLOR_WHITE);
    }
}

//the two for's draws the vertical and horizontal lines of the grid, respectivetly
void draw_grid(SDL_Surface* surface) {
    for(int i=0; i<COLUMNS; i++) {
        SDL_Rect column = (SDL_Rect) { i*CELL_SIZE, 0, LINE_WIDTH, SCREEN_HEIGHT };
        SDL_FillRect(surface, &column, COLOR_GRAY);
    }

    for(int j=0; j<ROWS; j++) {
        SDL_Rect row = (SDL_Rect) { 0, j*CELL_SIZE, SCREEN_WIDTH, LINE_WIDTH };
        SDL_FillRect(surface, &row, COLOR_GRAY);
    }
}

void initialize_environment(struct Cell environment[ROWS*COLUMNS]) {
    for(int i=0; i<ROWS; i++) {
        for(int j=0; j<COLUMNS; j++) {
            environment[j+COLUMNS*i] = (struct Cell) { WATER_TYPE, 0, j, i }; //representing a 2D array into a 1D array
        }
    }
}

void draw_environment(SDL_Surface* surface, struct Cell environment[ROWS*COLUMNS]) {
    for(int i=0; i<ROWS*COLUMNS; i++) {
        draw_cell(surface, environment[i]);
    }
}