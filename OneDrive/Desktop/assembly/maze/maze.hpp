#pragma once
#include <iostream>
#include <stack>
#include <windows.h>
#include "timer.hpp"
// grid size
#define HEIGHT 20
#define WIDTH  50

#define clearScreen() std::cout << "\033[H";

// maze
#define HEAD_COLOUR "\033[48;5;49m"
#define START_COLOUR "\033[48;5;196m"
#define WALL_COLOUR "\033[48;5;255m"
// a star
#define CURS_COLOUR "\033[48;5;49m"
#define TARGET_COLOUR "\033[48;5;196m"
#define PATH_COLOUR "\033[48;5;39m"

#define RESET "\033[m"

// control codes
#define ENTER 13
#define BACKSPACE 8
#define SPACE 32

typedef uint32_t uint32;

struct Maze_Node {
    int y, x;
    bool checked = false;
};

bool inRange(int y, int x);

struct Maze {
    char grid[HEIGHT][WIDTH];
   
    int initial_y, initial_x;   
    std::stack<Maze_Node> stack;

    bool show_back_tracking;
    bool animate;

    
    Maze(bool animate = false, bool show_back_tracking = false) {
        initial_y = rand() % HEIGHT;
        initial_x = rand() % WIDTH;
        stack.push({initial_y, initial_x, true});

        this->show_back_tracking = show_back_tracking;
        this->animate = animate;
    }
    
    void init();
    void print();
    
    void backTrack();
    void create(uint32 delay_ms);
};