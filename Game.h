#pragma once

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3_ttf/SDL_textengine.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <SDL3/SDL_rect.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <filesystem>
#include <format>

#include "Snake.h"
#include "Food.h"

using namespace std;

class Game {
    Snake mSnake;
    Food mFood;
    bool isPaused;
    bool isGameLost;
    size_t score;
    size_t highScore;
    int losses;
    SDL_FRect drawRect;
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;

    static constexpr const char* fontDirectory = "assets/fonts/";
    static constexpr const char* fontBitCountGridDouble = "BitcountGridDoubleInk-VariableFont_CRSV,ELSH,ELXP,SZP1,SZP2,XPN1,XPN2,YPN1,YPN2,slnt,wght.ttf";
    static constexpr const char* fontBitcountSingle = "BitcountSingle-VariableFont_CRSV,ELSH,ELXP,slnt,wght.ttf";
    static constexpr const char* fontSixtyfourConvergence = "SixtyfourConvergence-Regular-VariableFont_BLED,SCAN,XELA,YELA.ttf";
    static constexpr const char* fontWorkbenchRegular = "Workbench-Regular-VariableFont_BLED,SCAN.ttf";

    // fixed game textures
    SDL_Texture *textureGamePaused;
    SDL_Texture *textureYouWon;
    SDL_Texture *textureYouLost;
    SDL_Texture *textureSnakeLength;

    static constexpr int WINDOW_WIDTH = 640;
    static constexpr int WINDOW_HEIGHT = 480;
    static constexpr int TILE_SIZE = 20;
    
    static constexpr int SCOREBOARD_HEIGHT = 60;
    int BOARD_WIDTH_TILES;
    int BOARD_HEIGHT_TILES;


    SDL_Point randomTile() {
        return (SDL_Point){rand() % (BOARD_WIDTH_TILES - 1),
               rand() % (BOARD_HEIGHT_TILES - 1)};
    }
    
    public:
        Game();

        ~Game();

        int BoardWidth() const {
                return BOARD_WIDTH_TILES;
        }

        int BoardHeight() const {
            return BOARD_HEIGHT_TILES;
        }

        void resetGame();

        void handleGameLost();

        void replaceFood(size_t foodIndex);
       
        void ComposeScoreboardText();

        void ShowPaused();
       
        void ShowYouLost();
        
        void RenderScoreboard();

        void RenderSnake();

        void RenderSnakeBodySegment(int32_t game_x, int32_t game_y);

        void RenderSnakeHead(int32_t game_x, int32_t game_y);

        void DrawCircle(int32_t game_x, int32_t game_y,
                        bool filled, int32_t diameter);

        
        SDL_AppResult AppInit();

        SDL_AppResult handleAppEvent(void *, SDL_Event *event);

        SDL_AppResult Iterate(void *);
};

