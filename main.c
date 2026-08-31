// Included Libraries
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

// Global Definitions
#define  SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

// Game Definitions
#define COLS 12
#define ROWS 12

// Filling the screen with the most tiles given number of cols and rows
const int TILE_WIDTH = SCREEN_WIDTH / COLS;
const int TILE_HEIGHT = SCREEN_HEIGHT / ROWS;

// Defining the Tile
typedef struct{
    int x; // Row index
    int y; // Column index
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    int nearbyMineCount;
}sTILE;

// Creating the grid
sTILE grid[COLS][ROWS];
int revealedTilesCount;
int minesPresentCount;


// Function Prototypes
void GameStartup();
void GameUpdate();
void GameShutdown();
void GameRender();
void GameReset();

void RenderTiles();
void RenderTile(sTILE);
void ResetTiles();
int CountNearbyMines(int, int);


// Game Functions
void GameStartup(){
    InitAudioDevice();

    GameReset();
}

void GameUpdate(){

}

void GameShutdown(){
    CloseAudioDevice();
}

void GameRender(){
    RenderTiles();
}

void GameReset(){
    ResetTiles();
}

void RenderTiles(){
    for(int i = 0; i < COLS; i++){
        for(int j = 0; j < ROWS; j++){
            RenderTile(grid[i][j]);
        }
    }
}

void RenderTile(sTILE tile){
    if(tile.isMine){
        DrawRectangle(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, RED);
    }
    else{
        DrawRectangle(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, LIGHTGRAY);
        if(tile.nearbyMineCount > 0){
            DrawText(TextFormat("%d", tile.nearbyMineCount), tile.x * TILE_WIDTH + 13, tile.y * TILE_HEIGHT + 4, TILE_HEIGHT - 8, DARKGRAY);
        }
    }
    DrawRectangleLines(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, WHITE);
}

void ResetTiles(){

    // Resetting the tiles
    for(int i = 0; i < COLS; i++){
        for(int j = 0; j < ROWS; j++){
            grid[i][j] = (sTILE)
            {
                .x = i,
                .y = j,
                .isMine = false,
                .isRevealed = false,
                .isFlagged = false,
                .nearbyMineCount = -1
            };
        }
    }

    // Placing the mines
    minesPresentCount = (int)(ROWS * COLS * 0.1f);
    int minesToPlace = minesPresentCount;
    while(minesToPlace > 0){
        int col = GetRandomValue(0, COLS);
        int row = GetRandomValue(0, ROWS);

        if(!grid[col][row].isMine){

            grid[col][row].isMine = true;
            minesToPlace--;
        }
    }

    for(int i = 0; i < COLS; i++){
        for(int j = 0; j < ROWS; j++){
            if(!grid[i][j].isMine){
                grid[i][j].nearbyMineCount = CountNearbyMines(i, j);
            }
        }
    }
}

int CountNearbyMines(int col, int row){
    int count = 0;
    for(int colOffset = -1; colOffset <= 1; colOffset++){
        for(int rowOffset = -1; rowOffset <= 1; rowOffset++){
            if(colOffset == 0 && rowOffset == 0){
                continue;
            }
            if(grid[col + colOffset][row + rowOffset].isMine){
                count++;
            }
        }
    }
    return count;
}



int main(){

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Minesweeper :)");
    SetTargetFPS(60);
    GameStartup();

    // Game loop
    while(!WindowShouldClose()){
        GameUpdate();

        BeginDrawing();
        ClearBackground(DARKBLUE);

        GameRender();

        EndDrawing();
    }

    GameShutdown();
    CloseWindow();
    return 0;
}
