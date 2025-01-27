#include "config.h"
#include "structs.h"

#ifndef SIMULATION_H
#define SIMULATION_H
void water_rule_one(struct Cell environment[ROWS * COLUMNS]);
void water_rule_two(struct Cell environment[ROWS * COLUMNS]);
void water_rule_three(struct Cell environment[ROWS * COLUMNS]);
void simulation_step(struct Cell environment[ROWS*COLUMNS]);
#endif