#include <SDL2/SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_BLUE 0x34c3eb
#define COLOR_GRAY 0x1f1f1f1f
#define CELL_SIZE 20
#define LINE_WIDTH 2
#define COLUMNS SCREEN_WIDTH/CELL_SIZE
#define ROWS SCREEN_HEIGHT/CELL_SIZE
#define SOLID_TYPE 1
#define WATER_TYPE 0

struct Cell {
    int type;
    double fill_level; //0 = empty, 1 = full
    int x;
    int y;
};

// struct CellFlow {
//     double flow_left;
//     double flow_right;
//     double flow_up;
//     double flow_down;
// };

void draw_cell(SDL_Surface* surface, struct Cell cell) {
    int pixel_x = cell.x*CELL_SIZE;
    int pixel_y = cell.y*CELL_SIZE;
    SDL_Rect cell_rect = (SDL_Rect) { pixel_x, pixel_y, CELL_SIZE, CELL_SIZE };

     SDL_FillRect(surface, &cell_rect, COLOR_BLACK); //BG color

    //water fill level
    if(cell.type == WATER_TYPE) {
        int water_height = cell.fill_level * CELL_SIZE;
        int empty_height = CELL_SIZE - water_height;
        SDL_Rect water_rect = (SDL_Rect) { pixel_x, pixel_y + empty_height, CELL_SIZE, water_height }; 
        SDL_FillRect(surface, &water_rect, COLOR_BLUE);
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

void simulation_step(struct Cell environment[ROWS*COLUMNS]) {
    struct Cell environment_next[ROWS*COLUMNS];

    for(int i=0; i<ROWS*COLUMNS; i++) {
        environment_next[i] = environment[i];
    }

    for(int i=0; i<ROWS; i++) {
        for(int j=0; j<COLUMNS; j++) {
            //Rule #1: watah' flows down
            struct Cell source_cell = environment[j+COLUMNS*i];

            if(source_cell.type == WATER_TYPE && i<ROWS-1) { //if the cell is water and it is not at the bottom of the window
                struct Cell destination_cell = environment[j+COLUMNS*(i+1)];
                
                if(destination_cell.fill_level < source_cell.fill_level
                && destination_cell.fill_level < 1.0) {
                    environment_next[j+COLUMNS*(i+1)].fill_level += source_cell.fill_level;
                    environment_next[j+COLUMNS*i].fill_level = environment_next[j+COLUMNS*(i+1)].fill_level > 1.0 ? environment_next[j+COLUMNS*(i+1)].fill_level - 1.0 : 0.0;
                    environment_next[j+COLUMNS*(i+1)].fill_level = environment_next[j+COLUMNS*(i+1)].fill_level > 1.0 ? 1.0 : environment_next[j+COLUMNS*(i+1)].fill_level;
                }
            }

            //Rule #2: watah' flowing left and right
            int below_full_or_solid = 0;
            if(i+1 == ROWS || environment[j+COLUMNS*(i+1)].fill_level >= 1 || environment[j+COLUMNS*(i+1)].type == SOLID_TYPE) {
                if(source_cell.type == WATER_TYPE && j>0) {
                    struct Cell destination_cell = environment[(j-1)+COLUMNS*i];
                    
                    if(destination_cell.type == WATER_TYPE && destination_cell.fill_level < source_cell.fill_level) {
                        double delta_fill = source_cell.fill_level - destination_cell.fill_level;
                        environment_next[j+COLUMNS*i].fill_level -= delta_fill / 3;
                        environment_next[(j-1)+COLUMNS*i].fill_level += delta_fill / 3;
                    }
                }

                if(source_cell.type == WATER_TYPE && j<COLUMNS-1) {
                    struct Cell destination_cell = environment[(j+1)+COLUMNS*i];
                    
                    if(destination_cell.fill_level < source_cell.fill_level) {
                        double delta_fill = source_cell.fill_level - destination_cell.fill_level;
                        environment_next[j+COLUMNS*i].fill_level -= delta_fill / 3;
                        environment_next[(j+1)+COLUMNS*i].fill_level += delta_fill / 3;
                    }
                }
            }
        }
    }

    for(int i=0; i<ROWS*COLUMNS; i++) {
        environment[i] = environment_next[i];
    }
}
 
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
                    double fill_level = delete_mode ? 0.0 : 1.0; 

                    if(delete_mode != 0) {
                        current_type = WATER_TYPE;    
                    }

                    struct Cell cell = (struct Cell) { current_type, fill_level, cell_x, cell_y };

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