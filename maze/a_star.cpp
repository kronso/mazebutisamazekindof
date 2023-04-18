#include <iostream>
#include <vector> 
#include <cmath>    // sqrt() and pow()
#include <conio.h>  // _khbit() and _getch()

#include "maze.hpp"
#include "timer.hpp"

// control codes
#define ENTER 13
#define BACKSPACE 8
#define SPACE 32

struct Grid {
    char grid[HEIGHT][WIDTH];

    uint32 start_y, start_x;
    uint32 target_y, target_x;

    void init() {
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                grid[i][j] = ' ';
            }
        }   
    }

    void initStartAndEnd() {
        int rand_i, rand_j;
        for (int i = 0; i < 2; i++) {
            rand_i = rand() % HEIGHT;
            rand_j = rand() % WIDTH;
            if (grid[rand_i][rand_j] == ' ') {
                if (i == 0) {
                    grid[rand_i][rand_j] = 'X';      
                    target_y = rand_i;
                    target_x = rand_j; 
                } else {
                    grid[rand_i][rand_j] = 'O';
                    start_y = rand_i;
                    start_x = rand_j;
                }
            } else {
                i--;
            }
        }
    }

    // random walls placed n times
    void noise(int n) {
        int rand_i, rand_j;
        for (int i = 0; i < n; i++) {
            rand_i = rand() % HEIGHT;
            rand_j = rand() % WIDTH;
            if (grid[rand_i][rand_j] != 'X' && grid[rand_i][rand_j] != 'O') {
                grid[rand_i][rand_j] = '#';
            }
        }
    }
    
    // https://en.wikipedia.org/wiki/ANSI_escape_code
    uint32 costGradient(uint32 g) {
        switch(g) {
            case 208: case 209: case 210: case 211: case 212:
                g++;
            break;
            case 213: case 177: case 141: case 105: case 69:
                g -= 36;
            break;
            case 33: case 32: case 31: case 30: case 29:
                g--;
            break;
            case 28: case 64: case 100: case 136: case 172:
                g += 36;
            break;
        }
        return g;
    }
    
    void print() {
        clearScreen();
        uint32 gradient = 208;
        for (uint32 i = 0; i < HEIGHT; i++) {
            gradient = costGradient(gradient);
            for (uint32 j = 0; j < WIDTH; j++) {
                switch(grid[i][j]) {
                    case 'X': std::cout << TARGET_COLOUR << "  " << RESET; break;
                    case 'O': std::cout << PATH_COLOUR << "  " << RESET; break;
                    case ' ': std::cout << WALL_COLOUR << "  " << RESET; break;
                    case '@': std::cout << "\033[48;5;" << gradient << 'm' << "  " << RESET; break;
                    case '#': std::cout << "  "; break;
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        std::cout << TARGET_COLOUR << "  " << RESET << " Target Block" << std::endl;
        std::cout << PATH_COLOUR << "  " << RESET << " Starting Block" << std::endl << std::endl;
    }
};

struct aStar_Node {
    uint32 current_y, current_x;
    float global, local;
    int parent_y, parent_x;
    aStar_Node(uint32 y, uint32 x) {
        global = INFINITY;
        local = INFINITY;
        current_y = y;
        current_x = x;
        parent_y = -1;
        parent_x = -1;
    }
};

void initPositions(std::vector<aStar_Node> &graph) {
    for (uint32 i = 0; i < HEIGHT; i++) {
        for (uint32 j = 0; j < WIDTH; j++) {
            graph.push_back(aStar_Node(i, j));
        }
    }
}

// updates the global and local of the neighbour_aStar_Nodes
// are integers so it can do subtractions
float euclideanDist(aStar_Node &aStar_Node_a, aStar_Node &aStar_Node_b) {
    int rise = aStar_Node_b.current_y - aStar_Node_a.current_y;
    int run = aStar_Node_b.current_x - aStar_Node_a.current_x;
    return sqrt(pow(rise, 2) + pow(run, 2));
}

// sets values of given nodes
void updateaStar_Node(aStar_Node &current_aStar_Node, aStar_Node &neighbour_aStar_Node, aStar_Node &target_aStar_Node) {
    float distance = euclideanDist(current_aStar_Node, neighbour_aStar_Node);
    neighbour_aStar_Node.local = distance + current_aStar_Node.local;

    distance = euclideanDist(neighbour_aStar_Node, target_aStar_Node);
    neighbour_aStar_Node.global = distance + neighbour_aStar_Node.local;
    
    neighbour_aStar_Node.parent_y = current_aStar_Node.current_y;
    neighbour_aStar_Node.parent_x = current_aStar_Node.current_x;
}

// GET START aStar_Node AND TARGET aStar_Node
aStar_Node &getaStar_Node(std::vector<aStar_Node> &graph, Grid &grid, bool isTarget) {
    for (auto it = graph.begin(); it < graph.end(); it++) {
        if (isTarget) {
            if (it->current_y == grid.target_y && it->current_x == grid.target_x) {
                return *it;
            }
        } else {
            if (it->current_y == grid.start_y && it->current_x == grid.start_x) {
                return *it;
            }
        }
    }
    return graph.at(-1);
}

// stores a key of (y, x) and a value it's the global
struct Test_Node {
    uint32 y, x;
    float global;  
};

// just to see what the Test_Node contains
void print_T_nodes(std::vector<Test_Node> &t_nodes) {
    for (auto it: t_nodes) {
        std::cout << "Position: " << "("<< it.y << ", " << it.x << ")";
        std::cout << "\tGlobal: " << it.global << std::endl;
    }
}

// changes the previous starting aStar_Node to a new one 
aStar_Node &newCurrentaStar_Node(std::vector<aStar_Node> &graph, uint32 y, uint32 x) {
    for (auto it = graph.begin(); it < graph.end(); it++) {
        if (it->current_y == y && it->current_x == x) {
            return *it;
        }
    }
    return graph.at(-1);
}

// sort the Test_Nodes according to its global in ascending order
// this can determine the shortest path from the starting aStar_Node to the target aStar_Node
void bubbleSortVector(std::vector<Test_Node> &t_nodes) {
    for (auto it = t_nodes.begin(); it != t_nodes.end(); ++it) {
        for (auto jt = t_nodes.begin(); jt != t_nodes.end(); ++jt) {
            if (jt->global > it->global) {
                float temp = it->global;
                it->global = jt->global;
                jt->global = temp;

                uint32 temp_y, temp_x;      
                temp_y = it->y;
                it->y = jt->y;
                jt->y = temp_y;

                temp_x = it->x;
                it->x = jt->x;
                jt->x = temp_x;
            }
        }
    }
}

void eraseTaStar_Node(std::vector<Test_Node> &t_nodes, aStar_Node &aStar_Node) {
    for (auto it = t_nodes.begin(); it < t_nodes.end(); it++) {
        if (it->y == aStar_Node.current_y && it->x == aStar_Node.current_x) {
            t_nodes.erase(it);
        }
    }
}

void aStar(Grid &grid, uint32 delay_ms) {
    std::cout << "\033c";

    Maze maze(true, false); // (animate maze, show back-tracking)
    maze.init();
    maze.create(delay_ms);
    Sleep(1000);
    memcpy(grid.grid, maze.grid, HEIGHT * WIDTH * sizeof(char)); // copies the created maze into the grid
    grid.initStartAndEnd();

    // unhighlight if you want a flat board
    // highlight above as well
    // Grid grid;
    // grid.init();
    // grid.initStartAndEnd();
 
    std::vector<aStar_Node> graph;
    initPositions(graph);

    aStar_Node &start_aStar_Node  = getaStar_Node(graph, grid, false); 
    aStar_Node &target_aStar_Node = getaStar_Node(graph, grid, true); 
    
    // inititalise start_aStar_Node
    start_aStar_Node.local = 0;
    start_aStar_Node.global = euclideanDist(start_aStar_Node, target_aStar_Node); 
    start_aStar_Node.parent_y = start_aStar_Node.current_y;
    start_aStar_Node.parent_x = start_aStar_Node.current_x;

    // essentially dict with keys as coords and globals as values 
    // map the coords to its global
    std::vector<Test_Node> t_nodes;
    bool found = false;

    Timer t;
    while (!found) {
        for (auto it = graph.begin(); it < graph.end(); it++) {
            // this loop searches around its position
            for (int i = -1; i < 2; i++) {      
                for (int j = -1; j < 2; j++) {
                    if (it->current_y == start_aStar_Node.current_y + i && it->current_x == start_aStar_Node.current_x + j) {
                        if (start_aStar_Node.local + euclideanDist(start_aStar_Node, *it) < it->local && grid.grid[it->current_y][it->current_x] != '#') {
                            
                            updateaStar_Node(start_aStar_Node, *it, target_aStar_Node);        // will update attributes of aStar_Node
                            
                            t_nodes.push_back({it->current_y, it->current_x, it->global});     // store coords as keys and global as value

                            if (grid.grid[it->current_y][it->current_x] == 'X') {              // conditions to finish algorithm
                                found = true;
                            } else if (grid.grid[it->current_y][it->current_x] == ' ') {       // shows where the node has searched
                                grid.grid[it->current_y][it->current_x] = '@';
                            }
    
                            grid.print();
                            std::cout << "Delay: " << delay_ms << " ms" << std::endl;
                            // print_T_nodes(t_nodes);
                            // std::cout << "\nCurrent Local value: " << start_aStar_Node.local << "       Current Global value: " << start_aStar_Node.global << std::endl;
                            // std::cout << "\nNeighbour Local value: " << it->local << "      Neighbour Global value: " << it->global << std::endl;
                            Sleep(delay_ms);

                        }

                    }
                }   
            }

        }
        eraseTaStar_Node(t_nodes, start_aStar_Node);
        if (!t_nodes.empty()) {
            // sort the list to find the key that has the shortest path/global
            // ascending order
            bubbleSortVector(t_nodes);
            start_aStar_Node = newCurrentaStar_Node(graph, t_nodes.begin()->y, t_nodes.begin()->x);
        } else {
            std::cout << "Cannot reach target node. L";
            Sleep(1000);
            break;  
        }
    }
    // for (auto it = graph.rbegin(); it < graph.rend(); it++) {
    //     if (it->parent_y != -1 && it -> parent_x != -1) {
    //         grid.grid[it->parent_y][it->parent_x] = 'O';
    //             grid.print();
    //     }
    // }
    grid.print();
}

int main() {
    srand(time(NULL));
    // visualisation of the A* algorithim except it doesn't work properly :L (doesn't show the shortest path...)
    // uses euclidean distance
    // it looks cool though
    while (1) {
        Grid grid;
        aStar(grid, 0);
        Sleep(1000);
    }
    return 0;
}