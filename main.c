#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

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
#define DELAY_TIME 30

struct Cell {
    int type;
    double fill_level; //0 = empty, 1 = full
    int x;
    int y;
};

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

void watah_rule_one(struct Cell environment[ROWS*COLUMNS]) {
    struct Cell environment_next[ROWS * COLUMNS] = {0}; // Initialize the next state

    // Copy the initial state to the next state
    for (int i = 0; i < ROWS * COLUMNS; i++) {
        environment_next[i] = environment[i];
    }

    // Iterate through each cell
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            // Rule #1: Water flows down
            struct Cell source_cell = environment[j + COLUMNS * i];

            if (source_cell.type == WATER_TYPE && i < ROWS - 1) { // Check not at the bottom
                struct Cell destination_cell = environment[j + COLUMNS * (i + 1)];

                if (destination_cell.type == WATER_TYPE && destination_cell.fill_level < 1.0) {
                    // Calculate transferable amount
                    double transferable_amount = fmin(source_cell.fill_level, 1.0 - destination_cell.fill_level);

                    // Update the next state
                    environment_next[j + COLUMNS * (i + 1)].fill_level += transferable_amount;
                    environment_next[j + COLUMNS * i].fill_level -= transferable_amount;
                }
            }
        }
    }

    // Apply the next state to the current environment
    for (int i = 0; i < ROWS * COLUMNS; i++) {
        environment[i] = environment_next[i];
    }
}

void watah_rule_two(struct Cell environment[ROWS*COLUMNS]) {
    struct Cell environment_next[ROWS*COLUMNS];
    int rate_watah_move = 3;

    for(int i=0; i<ROWS*COLUMNS; i++) {
        environment_next[i] = environment[i];
    }

    for(int i=0; i<ROWS; i++) {
        for(int j=0; j<COLUMNS; j++) {
            //Rule #2: watah' flowing left and right
            struct Cell source_cell = environment[j+COLUMNS*i];

            //if cell below is bottom of boundary, completely full or solid
            if(i+1 == ROWS || environment[j+COLUMNS*(i+1)].fill_level >= 1 || environment[j+COLUMNS*(i+1)].type == SOLID_TYPE) {
                //Flow left
                if(source_cell.type == WATER_TYPE && j>0) { //j>0 not in the leftmost boundary
                    struct Cell destination_cell = environment[(j-1)+COLUMNS*i];
                    
                    if(destination_cell.type == WATER_TYPE && destination_cell.fill_level < source_cell.fill_level) {
                        //double delta_fill = source_cell.fill_level - destination_cell.fill_level;
                        // environment_next[j+COLUMNS*i].fill_level -= delta_fill / 3;
                        // environment_next[(j-1)+COLUMNS*i].fill_level += delta_fill / 3;
                        
                        // Calculate transferable amount
                        double transferable_amount = source_cell.fill_level - destination_cell.fill_level;

                        // Update the next state
                        environment_next[(j-1)+COLUMNS*i].fill_level += transferable_amount / rate_watah_move;
                        environment_next[j+COLUMNS*i].fill_level -= transferable_amount / rate_watah_move;
                        environment_next[j+COLUMNS*i].fill_level = environment_next[j+COLUMNS*i].fill_level <= 0.02 ? 0.0 : environment_next[j+COLUMNS*i].fill_level; //so the water does not divide infinitesimally
                    }
                }

                //Flow right
                if(source_cell.type == WATER_TYPE && j<COLUMNS-1) {
                    struct Cell destination_cell = environment[(j+1)+COLUMNS*i];
                    
                    if(destination_cell.type == WATER_TYPE && destination_cell.fill_level < source_cell.fill_level) {
                    //     //     double delta_fill = source_cell.fill_level - destination_cell.fill_level;
                    //     //     environment_next[j+COLUMNS*i].fill_level -= delta_fill / 3;
                    //     //     environment_next[(j+1)+COLUMNS*i].fill_level += delta_fill / 3;
                        // Calculate transferable amount
                        double transferable_amount = source_cell.fill_level - destination_cell.fill_level;

                        // Update the next state
                        environment_next[(j+1)+COLUMNS*i].fill_level += transferable_amount / rate_watah_move;
                        environment_next[j+COLUMNS*i].fill_level -= transferable_amount / rate_watah_move;
                        environment_next[j+COLUMNS*i].fill_level = environment_next[j+COLUMNS*i].fill_level < 0.02 ? 0.0 : environment_next[j+COLUMNS*i].fill_level;
                    }
                }
            }
        }
    }

    for(int i=0; i<ROWS*COLUMNS; i++) {
        environment[i] = environment_next[i];
    }
}

void simulation_step(struct Cell environment[ROWS*COLUMNS]) {
    watah_rule_one(environment);
    watah_rule_two(environment);
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

        SDL_Delay(DELAY_TIME);
    }

    return 0;
}