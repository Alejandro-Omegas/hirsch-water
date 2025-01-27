
#include <math.h>
#include "structs.h"
#include "config.h"

void water_rule_one(struct Cell environment[ROWS * COLUMNS]) {
    struct Cell environment_next[ROWS * COLUMNS];

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

void water_rule_two(struct Cell environment[ROWS * COLUMNS]) {
    struct Cell environment_next[ROWS*COLUMNS];

    // Copy current environment to environment_next
    for(int i=0; i<ROWS*COLUMNS; i++) {
        environment_next[i] = environment[i];
    }

    for(int i=0; i<ROWS; i++) {
        for(int j=0; j<COLUMNS; j++) {
            struct Cell source_cell = environment[j + COLUMNS * i];

            // Check if water cannot flow downward
            if(i+1 == ROWS || environment[j + COLUMNS*(i+1)].fill_level >= 0.99 || //originally >= 1; changed cause it seems it can form columns of water under certain conditions
               environment[j + COLUMNS*(i+1)].type == SOLID_TYPE) {

                // Flow left
                if(source_cell.type == WATER_TYPE && j > 0) {
                    struct Cell destination_cell = environment[(j-1) + COLUMNS * i];
                    
                    if(destination_cell.type == WATER_TYPE && 
                       destination_cell.fill_level < source_cell.fill_level) {
                        double transferable = (source_cell.fill_level - destination_cell.fill_level) / 3;
                        environment_next[(j-1) + COLUMNS * i].fill_level += transferable;
                        environment_next[j + COLUMNS * i].fill_level -= transferable;
                    }
                }

                // Flow right (FIXED: Added type check)
                if(source_cell.type == WATER_TYPE && j < COLUMNS - 1) {
                    struct Cell destination_cell = environment[(j+1) + COLUMNS * i];
                    
                    if(destination_cell.type == WATER_TYPE &&  // Added type check
                       destination_cell.fill_level < source_cell.fill_level) {
                        double transferable = (source_cell.fill_level - destination_cell.fill_level) / 3;
                        environment_next[(j+1) + COLUMNS * i].fill_level += transferable;
                        environment_next[j + COLUMNS * i].fill_level -= transferable;
                    }
                }
            }
        }
    }

    // Update environment with the next state
    for(int i=0; i<ROWS*COLUMNS; i++) {
        environment[i] = environment_next[i];
    }
}

void water_rule_three(struct Cell environment[ROWS * COLUMNS]) {
    struct Cell environment_next[ROWS*COLUMNS];

    for(int i=0; i<ROWS*COLUMNS; i++) {
        environment_next[i] = environment[i];
    }

    for(int i=0; i<ROWS; i++) {
        for(int j=0; j<COLUMNS; j++) {
            struct Cell source_cell = environment[j+COLUMNS*i];

            if(i>0
            && environment[j+COLUMNS*(i-1)].type == WATER_TYPE
            && source_cell.type == WATER_TYPE
            && source_cell.fill_level > 1.0
            && source_cell.fill_level > environment[j+COLUMNS*(i-1)].fill_level) {
                double transfer_fill = (source_cell.fill_level-1);
                environment_next[j+COLUMNS*i].fill_level -= transfer_fill;
                environment_next[j+COLUMNS*(i-1)].fill_level += transfer_fill;
            }
        }
    }

    for(int i=0; i<ROWS*COLUMNS; i++) {
        environment[i] = environment_next[i];
    }
}

void simulation_step(struct Cell environment[ROWS*COLUMNS]) {
    water_rule_one(environment);
    water_rule_two(environment);
    water_rule_three(environment);
}
 