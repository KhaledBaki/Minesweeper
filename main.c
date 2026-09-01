// Included Libraries
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

// Global Definitions
#define  SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

// Game Definitions
#define COLS 25
#define ROWS 25

// Game Difficulty
bool isEasy = true;
bool isMedium = false;
bool isHard = false;

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

// Game Variables
int revealedTilesCount;
int minesPresentCount;
float timeGameStarted;
float timeGameEnded;
bool isMusicEnabled = true;
bool isSoundEnabled = true;

const char* labelGameWin = "YOU WIN!";
const char* labelGameLose = "GAME OVER :(";
const char* labelEnter = "PRESS ENTER FOR MAIN MENU";

// Images and Icons
#define MAX_TEXTURES 1
typedef enum{
    TEXTURE_FLAG = 0
}texture_asset;

Texture2D textures[MAX_TEXTURES];

// Game States
typedef enum{
    STATE_MAIN_MENU = 0,
    STATE_OPTIONS_MENU,
    STATE_DIFFICULTY_GRID_MENU,
    STATE_PLAYING,
    STATE_LOSE,
    STATE_WIN
}game_states;

game_states gameState;


// Game Sounds
#define MAX_SOUNDS 5
typedef enum{
    SOUND_ONE = 0,
    SOUND_TWO,
    SOUND_THREE,
    SOUND_FOUR,
    SOUND_FIVE
}sound_asset;

Sound sounds[MAX_SOUNDS];


// Game Music Tracks
#define MAX_MUSIC 1
typedef enum{
    MUSIC_ONE = 0
}music_asset;

Music music[MAX_MUSIC];

// Function Prototypes
void GameStartup();
void GameUpdate();
void GameShutdown();
void GameRender();
void GameReset();
void GamePlaySound(int sound);

void RenderTiles();
void RenderTile(sTILE);
void ResetTiles();
int CountNearbyMines(int, int);
bool IsTileIndexValid(int, int);
void RevealTile(int, int);
void FlagTile(int, int);
void RevealTilesFrom(int, int);
float difficultyValue();



// Game Functions
void GameStartup(){
    InitAudioDevice();

    Image image1 = LoadImage("assets/flag.png");
    textures[TEXTURE_FLAG] = LoadTextureFromImage(image1);
    UnloadImage(image1);

    sounds[SOUND_ONE] = LoadSound("assets/click.wav");
    sounds[SOUND_TWO] = LoadSound("assets/explosion.wav");
    sounds[SOUND_THREE] = LoadSound("assets/pickupCoin.wav");
    sounds[SOUND_FOUR] = LoadSound("assets/winner.mp3");
    sounds[SOUND_FIVE] = LoadSound("assets/lose.mp3");
    music[MUSIC_ONE] = LoadMusicStream("assets/8-bit-game-158815.mp3");

    PlayMusicStream(music[MUSIC_ONE]);


    gameState = STATE_MAIN_MENU;
}

void GameUpdate(){

    UpdateMusicStream(music[MUSIC_ONE]);

    switch (gameState){
    case STATE_MAIN_MENU:
        if(IsKeyPressed(KEY_N)){
            GamePlaySound(SOUND_TWO);
            GameReset();
        }
        else if (IsKeyPressed(KEY_O)){
            gameState = STATE_OPTIONS_MENU;
            GamePlaySound(SOUND_TWO);
        }
        else if (IsKeyPressed(KEY_D)){
            gameState = STATE_DIFFICULTY_GRID_MENU;
            GamePlaySound(SOUND_TWO);
        }
        break;
    
    case STATE_DIFFICULTY_GRID_MENU:
        if(IsKeyPressed(KEY_E)){
            isEasy = true;
            isMedium = false;
            isHard = false;
            GamePlaySound(SOUND_ONE);
        }
        else if(IsKeyPressed(KEY_M)){
            isEasy = false;
            isMedium = true;
            isHard = false;
            GamePlaySound(SOUND_ONE);
        }
        else if(IsKeyPressed(KEY_H)){
            isEasy = false;
            isMedium = false;
            isHard = true;
            GamePlaySound(SOUND_ONE);
        }
        else if(IsKeyPressed(KEY_ENTER)){
            gameState = STATE_MAIN_MENU;
            GamePlaySound(SOUND_TWO);
        }
        break;

    case STATE_OPTIONS_MENU:
        if(IsKeyPressed(KEY_ENTER)){
            gameState = STATE_MAIN_MENU;
            GamePlaySound(SOUND_TWO);
        }
        if(IsKeyPressed(KEY_S)){
            isSoundEnabled = !isSoundEnabled;
            GamePlaySound(SOUND_ONE);
        }
        if(IsKeyPressed(KEY_M)){
            isMusicEnabled = !isMusicEnabled;
            GamePlaySound(SOUND_ONE);
        
            if(isMusicEnabled){
                StopMusicStream(music[MUSIC_ONE]);
                PlayMusicStream(music[MUSIC_ONE]);
            }
            else{
                StopMusicStream(music[MUSIC_ONE]);
            }
        }

        break;
    
    case STATE_PLAYING:
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            Vector2 mousePos = GetMousePosition();
            int col = (int)(mousePos.x / TILE_WIDTH);
            int row = (int)(mousePos.y / TILE_HEIGHT);

            if(IsTileIndexValid(col, row)){
                RevealTile(col, row);
                GamePlaySound(SOUND_ONE);
            }
        } else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
            Vector2 mousePos = GetMousePosition();
            int col = (int)(mousePos.x / TILE_WIDTH);
            int row = (int)(mousePos.y / TILE_HEIGHT);

            if(IsTileIndexValid(col, row)){
                FlagTile(col, row);
            }
        }
        break;

    case STATE_LOSE:
        if(IsKeyPressed(KEY_ENTER)){
            GamePlaySound(SOUND_TWO);
            gameState = STATE_MAIN_MENU;
        }
        break;

    case STATE_WIN:
        if(IsKeyPressed(KEY_ENTER)){
            GamePlaySound(SOUND_TWO);
            gameState = STATE_MAIN_MENU;
        }
        break;
    }
}

void GameShutdown(){
    for(int i = 0; i < MAX_TEXTURES; i++){
        UnloadTexture(textures[i]);
    }

    for(int i = 0; i < MAX_SOUNDS; i++){
        UnloadSound(sounds[i]);
    }

    StopMusicStream(music[MUSIC_ONE]);
    UnloadMusicStream(music[MUSIC_ONE]);

    CloseAudioDevice();
}

void GameRender(){
    int seconds;
    int minutes;
    switch (gameState)
    {
    case STATE_MAIN_MENU:
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DARKBLUE);
        DrawText("MINESWEEPER", 20, 20, 75, WHITE);
        DrawText("[N]ew Game", 120, 220, 40, WHITE);
        DrawText("[O]ptions", 120, 270, 40, WHITE);
        DrawText("[D]ifficulty", 120, 320, 40, WHITE);
        DrawText("ESC to QUIT", 120, 370, 40, WHITE);
        DrawText("Copyright 2024 @eyuzwa", 20, 500, 20, WHITE);
        break;
        
    case STATE_DIFFICULTY_GRID_MENU:
        DrawText("MINESWEEPER :: DIFFICULTY", 20, 20, 40, WHITE);

        DrawText("[E]asy ", 120, 220, 20, WHITE);
        DrawText("[M]edium ", 120, 250, 20, WHITE);
        DrawText("[H]ard ", 120, 280, 20, WHITE);
        if (isEasy) {
            DrawText("EASY", 280, 220, 20, YELLOW);
            DrawText(" / ", 310, 220, 20, WHITE);
            DrawText("MEDIUM", 350, 220, 20, WHITE);
            DrawText(" / ", 380, 220, 20, WHITE);
            DrawText("HARD", 420, 220, 20, WHITE);
        }
        else if (isMedium) {
            DrawText("EASY", 280, 220, 20, WHITE);
            DrawText(" / ", 310, 220, 20, WHITE);
            DrawText("MEDIUM", 350, 220, 20, YELLOW);
            DrawText(" / ", 380, 220, 20, WHITE);
            DrawText("HARD", 420, 220, 20, WHITE);
        }
        else if (isHard) {
            DrawText("EASY", 280, 220, 20, WHITE);
            DrawText(" / ", 310, 220, 20, WHITE);
            DrawText("MEDIUM", 350, 220, 20, WHITE);
            DrawText(" / ", 380, 220, 20, WHITE);
            DrawText("HARD", 420, 220, 20, YELLOW);
        }
        DrawText(labelEnter, 120, 400, 20, WHITE);
        break;

    case STATE_OPTIONS_MENU:
    DrawText("MINESWEEPER :: OPTIONS", 20, 20, 40, WHITE);

        DrawText("[S]ound ", 120, 220, 20, WHITE);
        if (isSoundEnabled) {
            DrawText("ON", 280, 220, 20, YELLOW);
            DrawText(" / ", 310, 220, 20, WHITE);
            DrawText("OFF", 350, 220, 20, WHITE);
        }
        else {
            DrawText("ON", 280, 220, 20, WHITE);
            DrawText(" / ", 310, 220, 20, WHITE);
            DrawText("OFF", 350, 220, 20, YELLOW);
        }

        DrawText("[M]usic", 120, 250, 20, WHITE);
        if (isMusicEnabled) {
            DrawText("ON", 280, 250, 20, YELLOW);
            DrawText(" / ", 310, 250, 20, WHITE);
            DrawText("OFF", 350, 250, 20, WHITE);
        }
        else {
            DrawText("ON", 280, 250, 20, WHITE);
            DrawText(" / ", 310, 250, 20, WHITE);
            DrawText("OFF", 350, 250, 20, YELLOW);
        }
        DrawText(labelEnter, 120, 400, 20, WHITE);
        break;
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DARKBLUE);
        DrawText("MINESWEEPER :: OPTIONS", 20, 20, 40, WHITE);

        DrawText("[S]ound ", 120, 220, 20, WHITE);
        if (isSoundEnabled) {
            DrawText("ON", 280, 220, 20, YELLOW);
            DrawText(" / ", 310, 220, 20, WHITE);
            DrawText("OFF", 350, 220, 20, WHITE);
        }
        else {
            DrawText("ON", 280, 220, 20, WHITE);
            DrawText(" / ", 310, 220, 20, WHITE);
            DrawText("OFF", 350, 220, 20, YELLOW);
        }

        DrawText("[M]usic", 120, 250, 20, WHITE);
        if (isMusicEnabled) {
            DrawText("ON", 280, 250, 20, YELLOW);
            DrawText(" / ", 310, 250, 20, WHITE);
            DrawText("OFF", 350, 250, 20, WHITE);
        }
        else {
            DrawText("ON", 280, 250, 20, WHITE);
            DrawText(" / ", 310, 250, 20, WHITE);
            DrawText("OFF", 350, 250, 20, YELLOW);
        }
        DrawText(labelEnter, 120, 400, 20, WHITE);
        break;
    
    case STATE_PLAYING:
        RenderTiles();
        break;

    case STATE_LOSE:
        RenderTiles();
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(WHITE, 0.8f));
        DrawText(labelGameLose, SCREEN_WIDTH / 2 - MeasureText(labelGameLose, 60) / 2, SCREEN_HEIGHT / 2 - 10, 60, DARKGRAY);
        DrawText(labelEnter, SCREEN_WIDTH / 2 - MeasureText(labelEnter, 34) / 2, (int)(SCREEN_HEIGHT * 0.75f) - 10, 34, DARKGRAY);
        
        seconds = (int)(timeGameEnded - timeGameStarted) & 60;
        DrawText(TextFormat("TIME PLAYED: %d s", seconds), 20, 40, 34, DARKGRAY);
        
        break;

    case STATE_WIN:
        RenderTiles();
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(WHITE, 0.8f));
        DrawText(labelGameWin, SCREEN_WIDTH / 2 - MeasureText(labelGameWin, 60) / 2, SCREEN_HEIGHT / 2 - 10, 60, DARKGRAY);
        DrawText(labelEnter, SCREEN_WIDTH / 2 - MeasureText(labelEnter, 34) / 2, (int)(SCREEN_HEIGHT * 0.75f) - 10, 34, DARKGRAY);
        
        seconds = (int)(timeGameEnded - timeGameStarted) & 60;
        DrawText(TextFormat("TIME PLAYED: %d s", seconds), 20, 40, 34, DARKGRAY);
        
        break;
    }
    
}

void GameReset(){
    ResetTiles();
    gameState = STATE_PLAYING;
    revealedTilesCount = 0;
}

void RenderTiles(){
    for(int i = 0; i < COLS; i++){
        for(int j = 0; j < ROWS; j++){
            RenderTile(grid[i][j]);
        }
    }
}

void RenderTile(sTILE tile){
    if(tile.isRevealed){
        if(tile.isMine){
            DrawRectangle(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, RED);
        }
        else{
            DrawRectangle(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, LIGHTGRAY);
            if(tile.nearbyMineCount > 0){
                DrawText(TextFormat("%d", tile.nearbyMineCount), tile.x * TILE_WIDTH + 13, tile.y * TILE_HEIGHT + 4, TILE_HEIGHT - 8, DARKGRAY);
            }
        }
    } else if(tile.isFlagged){
        Rectangle source = {0, 0, (float)textures[TEXTURE_FLAG].width, (float)textures[TEXTURE_FLAG].height};
        Rectangle dest = {(float)(tile.x * TILE_WIDTH), (float)(tile.y * TILE_HEIGHT), (float)TILE_WIDTH, (float)TILE_HEIGHT};
        Vector2 origin = {0, 0};

        DrawTexturePro(textures[TEXTURE_FLAG], source, dest, origin, 0.0f, WHITE);

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
    minesPresentCount = (int)(ROWS * COLS * difficultyValue());
    int minesToPlace = minesPresentCount;
    while(minesToPlace > 0){
        int col = GetRandomValue(0, COLS - 1);
        int row = GetRandomValue(0, ROWS - 1);

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
            if(IsTileIndexValid(col + colOffset, row + rowOffset)){
                if(grid[col + colOffset][row + rowOffset].isMine){
                    count++;
                }
            }
        }
    }
    return count;
}

bool IsTileIndexValid(int col, int row){
    return col >= 0 && col < COLS && row >= 0 && row < ROWS;
}

void RevealTile(int col, int row){
    if(grid[col][row].isFlagged || grid[col][row].isRevealed){
        return;
    }

    grid[col][row].isRevealed = true;

    if(grid[col][row].isMine){
        gameState = STATE_LOSE;
        timeGameEnded = (float)GetTime();
        GamePlaySound(SOUND_FIVE);

    }
    else{
        if (grid[col][row].nearbyMineCount == 0){
            
            // Recursive call
            RevealTilesFrom(col, row);
        }

        revealedTilesCount++;

        if(revealedTilesCount >= (ROWS * COLS) - minesPresentCount){
            gameState = STATE_WIN;
            timeGameEnded = (float)GetTime();
            GamePlaySound(SOUND_FOUR);
        }
    }
}

void FlagTile(int col, int row){
    // Toggle
    grid[col][row].isFlagged = !grid[col][row].isFlagged;
    GamePlaySound(SOUND_THREE);

}

void RevealTilesFrom(int col, int row){
    for(int colOffset = -1; colOffset <= 1; colOffset++){
        for(int rowOffset = -1; rowOffset <= 1; rowOffset++){
            if(colOffset == 0 && rowOffset == 0){
                continue;
            }
            if(!IsTileIndexValid(col + colOffset, row + rowOffset)){
                continue;
            }
            
            RevealTile(col + colOffset, row + rowOffset);
        }
    }
}

void GamePlaySound(int sound){
    if(isSoundEnabled){
        PlaySound(sounds[sound]);
    }
}

float difficultyValue(){
    if(isEasy){
        return 0.1f;
    }
    else if (isMedium){
        return 0.3f;
    }
    return 0.4f;
}
int main(){

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Minesweeper :)");
    
    Image icon = LoadImage("resources/icon.png");
    ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8); // Reformating the image
    SetWindowIcon(icon);
    UnloadImage(icon);

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
