#ifndef STRUCTS_H
#define STRUCTS_H
struct Cell {
    int type;
    double fill_level; //0 = empty, 1 = full
    int x;
    int y;
};
#endif