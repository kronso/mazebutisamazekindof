#include <iostream>
#include <stack>
#include <windows.h>
#include <conio.h>

#include "maze.hpp"
#include "timer.hpp"

bool inRange(int y, int x) {
    if (y < HEIGHT && y > -1) {
        if (x < WIDTH && x > -1) {
            return true;
        }
    }
    return false;
}
void Maze::init() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            grid[i][j] = '#';
        }
    }   
}
void Maze::print() {
    clearScreen();
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == stack.top().y && j == stack.top().x) {
                std::cout << HEAD_COLOUR << "  " << RESET;            
            } else if (grid[i][j] == ' ') {
                std::cout << WALL_COLOUR << "  " << RESET;            
            } else {
                std::cout << "  ";            
            }
        }
        std::cout << std::endl;
    }
}
void Maze::backTrack() {
    bool down  = false;
    bool up    = false;
    bool right = false;
    bool left  = false;
    if (!inRange(stack.top().y + 2, stack.top().x)) {
        down = true;
    } else if (grid[stack.top().y + 2][stack.top().x] == ' ') {
        down = true;
    }

    if (!inRange(stack.top().y - 2, stack.top().x)) {
        up = true;
    } else if (grid[stack.top().y - 2][stack.top().x] == ' ') {
        up = true;
    }

    if (!inRange(stack.top().y, stack.top().x + 2)) {
        right = true;
    } else if (grid[stack.top().y][stack.top().x + 2] == ' ') {
        right = true;
    }


    if (!inRange(stack.top().y, stack.top().x - 2)) {
        left = true;
    } else if (grid[stack.top().y][stack.top().x - 2] == ' ') {
        left = true;
    }


    if (down && up && right && left) {
        stack.pop();
        stack.pop();
        if (show_back_tracking && animate) {print();} // visualises the back-tracking
        backTrack();
    }
}
void Maze::create(uint32 delay_ms) {
    int location[2] = {-1, 1};
    int direction = location[rand() % 2];
    int rand_choice =  rand() % 2;        // gives a 50/50 chance to do either
    switch(rand_choice) {
        case 0:
            if (inRange(stack.top().y + direction + direction, stack.top().x)) {
                if (grid[stack.top().y + direction + direction][stack.top().x] != ' ') {   // check if the North + 1 and South + 1 node is not checked

                    grid[stack.top().y + direction][stack.top().x] = ' ';                  // if so, print to the grid
                    stack.push({stack.top().y + direction, stack.top().x, true});          // and push onto the stack
                    // print();

                    if (grid[stack.top().y + direction][stack.top().x] != ' ') {           // check if the North and South node is not checked

                        grid[stack.top().y + direction][stack.top().x] = ' ';              // if so, print to the grid
                        stack.push({stack.top().y + direction, stack.top().x, true});      // and push onto the stack
                        if (animate) { print(); Sleep(delay_ms); }
                    }
                }
            }
            break;
        case 1: 
            if (inRange(stack.top().y, stack.top().x + direction + direction)) {
                if (grid[stack.top().y][stack.top().x + direction + direction] != ' ') {   // check if the East + 1 and West + 1 node is not checked

                    grid[stack.top().y][stack.top().x + direction] = ' ';                  // if so, print to the grid
                    stack.push({stack.top().y, stack.top().x + direction, true});          // and push onto the stack
                    // print();

                    if (grid[stack.top().y][stack.top().x + direction] != ' ') {           // check if the East and West node is not checked

                        grid[stack.top().y][stack.top().x + direction] = ' ';              // if so, print to the grid
                        stack.push({stack.top().y, stack.top().x + direction, true});      // and push onto the stack
                        if (animate) { print(); Sleep(delay_ms); }
                    }   
                }
            }
            break;
    }
    if (stack.top().y == initial_y && stack.top().x == initial_x && stack.size() > 2) { return; }
    backTrack();    
    create(delay_ms);
};
