#include "raylib.h"
#include <vector>
#include <cstdlib>
#include "game_states.h"
extern GameState currentGameState;


using namespace std;

const int screenWidth = 1200;
const int screenHeight = 800;
const int gridSize = 5;
int rows = screenHeight / gridSize;
int cols = screenWidth / gridSize;

vector<vector<pair<pair<int, int>, int>>> cells;
vector<vector<pair<pair<int, int>, int>>> tempCells;

Color green = {0, 255, 0, 255};
Color grey = {29, 29, 29, 255};

class grid_simulation {
public:
    vector<pair<int, int>> offset = {
        {-1, -1}, {-1, 0}, {-1, 1},
        { 0, -1},          { 0, 1},
        { 1, -1}, { 1, 0}, { 1, 1}
    };

    void init_cells() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                cells[i][j].first = {i, j};
                int random = rand() % 5;
                cells[i][j].second = (random == 4) ? 1 : 0;
            }
        }
    }

    void draw_cells() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                Color toDraw = (cells[i][j].second == 1) ? green : grey;
                DrawRectangle(j * gridSize, i * gridSize, gridSize - 1, gridSize - 1, toDraw);
            }
        }
    }

    int count_neighbors(int i, int j) {
        int numbers = 0;
        for (auto it : offset) {
            int ni = (i + it.first + rows) % rows;
            int nj = (j + it.second + cols) % cols;
            if (cells[ni][nj].second == 1) numbers++;
        }
        return numbers;
    }

    void simulate() {
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                tempCells[i][j].second = 0;

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                int live = count_neighbors(i, j);
                if (cells[i][j].second == 1) {
                    if (live < 2 || live > 3)
                        tempCells[i][j].second = 0;
                    else
                        tempCells[i][j].second = 1;
                } else {
                    if (live == 3)
                        tempCells[i][j].second = 1;
                }
            }
        }
    }
};

void PlayGAMEOFLIFE() {
    static bool initialized = false;
    static grid_simulation grids;
    if (IsKeyPressed(KEY_ESCAPE)) {
    initialized = false;
    currentGameState = MAIN_ROOM;
    return;
}


    if (!initialized) {
        rows = GetScreenHeight() / gridSize;
        cols = GetScreenWidth() / gridSize;
        cells.resize(rows, vector<pair<pair<int, int>, int>>(cols, {{0, 0}, 0}));
        tempCells.resize(rows, vector<pair<pair<int, int>, int>>(cols, {{0, 0}, 0}));
        grids.init_cells();
        initialized = true;
    }

    grids.draw_cells();
    grids.simulate();
    cells = tempCells;
}

