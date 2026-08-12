#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <filesystem>
#include <format>

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#include "Game.h"
#include "Snake.h"
#include "Food.h"


    
Game::Game()
            : isPaused(false),
            isGameLost(false),
            score(0),
            highScore(0),
            losses(0),
            renderer(nullptr),
            font(nullptr),
            textureGamePaused(nullptr),
            textureYouWon(nullptr),
            textureYouLost(nullptr),
            BOARD_WIDTH_TILES(WINDOW_WIDTH / TILE_SIZE),
            BOARD_HEIGHT_TILES((WINDOW_HEIGHT - SCOREBOARD_HEIGHT) / TILE_SIZE)
{
}

Game::~Game() {
}

void Game::resetGame() {
    score = 0;
    mFood.Reset();
    for(int f = 0; f < (rand() % 4) + 3; f++) mFood.addToFood(randomTile());

    mSnake.Reset(randomTile());
}

void Game::handleGameLost() {
    isGameLost = true;

    if (score > highScore ) {
        highScore = score;
        ComposeScoreboardText();
    }
}

void Game::replaceFood(size_t foodIndex) {

    bool foodLocationFound = false;
    SDL_Point potentialFood;

    while (!foodLocationFound) {

        potentialFood = randomTile();

        if (mFood.ContainsPoint(potentialFood) >= 0) {
            printf("food double prevented\n");
            continue;
        }

        if (!mSnake.ContainsPoint(potentialFood)) {
            foodLocationFound = true;
        }
    }

    mFood.foodStorage[foodIndex] = potentialFood;
}

void Game::ComposeScoreboardText() {

    SDL_Color color = { 255, 255, 255, SDL_ALPHA_OPAQUE };
    SDL_Surface *text;
    std::string banner = std::format("Score : {}                  High score : {}", score, highScore);

    text = TTF_RenderText_Blended(font, banner.c_str(), 0, color); 
    if (text) {
        textureSnakeLength = SDL_CreateTextureFromSurface(renderer, text);
        SDL_DestroySurface(text);
    }
    if (!textureSnakeLength) {
        SDL_Log("Couldn't create text: %s\n", SDL_GetError());
    }
}


void Game::ShowPaused() {
    int w = 0, h = 0;
    SDL_FRect dst;
    const float scale = 1.0f;


    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    SDL_GetTextureSize(textureGamePaused, &dst.w, &dst.h);
    dst.x = ((w / scale) - dst.w) / 2;
    dst.y = ((h / scale) - dst.h) / 2;

    /* Draw the text */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //            SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, textureGamePaused, NULL, &dst);


}

void Game::ShowYouLost() {
    int w = 0, h = 0;
    SDL_FRect dst;
    const float scale = 1.0f;


    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    SDL_GetTextureSize(textureYouLost, &dst.w, &dst.h);
    dst.x = ((w / scale) - dst.w) / 2;
    dst.y = ((h / scale) - dst.h) / 2;

    /* Draw the text */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //            SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, textureYouLost, NULL, &dst);


}

void Game::RenderScoreboard() {

    SDL_FRect border;
    border.x = 0;
    border.y = 0;
    border.h = WINDOW_HEIGHT - SCOREBOARD_HEIGHT;
    border.w = WINDOW_WIDTH;


    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    //            SDL_RenderLine(renderer, 0, WINDOW_HEIGHT - SCOREBOARD_HEIGHT + 1, WINDOW_WIDTH, WINDOW_HEIGHT - SCOREBOARD_HEIGHT + 1);
    SDL_RenderRect(renderer, &border);



    int w = 0, h = 0;
    SDL_FRect dst;
    const float scale = 1.0f;

    dst.x = 5;
    dst.y = WINDOW_HEIGHT - SCOREBOARD_HEIGHT + 4;

    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    SDL_GetTextureSize(textureSnakeLength, &dst.w, &dst.h);

    /* Draw the text */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //            SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, textureSnakeLength, NULL, &dst);
}

void Game::RenderSnake() {
    SDL_Point mSeg;

    if (mSnake.Size() > 0) {

        // draw snake body
        for(size_t i = 1; i < mSnake.Size(); i++) {
            mSeg = mSnake.Segment(i);
            RenderSnakeBodySegment(mSeg.x, mSeg.y);
        }
        // draw snake head
        mSeg = mSnake.Segment(0);
        RenderSnakeHead(mSeg.x, mSeg.y);
    }
}

void Game::RenderSnakeBodySegment(int32_t game_x, int32_t game_y) {
    SDL_SetRenderDrawColor(renderer, 180, 50, 50, SDL_ALPHA_OPAQUE);
    DrawCircle(game_x, game_y, true, TILE_SIZE - 4);
}

     
void Game::RenderSnakeHead(int32_t game_x, int32_t game_y) {
    // std::string debug = std::format("DrawSnakeHead() game_x: {}   game_y: {}\n", game_x, game_y);
    // printf("%s", debug.c_str());
    SDL_SetRenderDrawColor(renderer, 255, 165, 0, SDL_ALPHA_OPAQUE);
    DrawCircle(game_x, game_y, true, TILE_SIZE);

    const int32_t centreX = game_x * TILE_SIZE + TILE_SIZE / 2;
    const int32_t centreY = game_y * TILE_SIZE + TILE_SIZE / 2;
    int32_t eyeX = centreX;
    int32_t eyeY = centreY;
    int32_t mouthStartX = centreX;
    int32_t mouthStartY = centreY;
    int32_t mouthEndX = centreX;
    int32_t mouthEndY = centreY;

    switch (mSnake.GetDirection()) {
        case UP:
            eyeX -= 3;
            eyeY -= 3;
            mouthStartX -= 4;
            mouthEndX += 4;
            mouthStartY = mouthEndY = centreY - 6;
            break;
        case DOWN:
            eyeX += 3;
            eyeY += 3;
            mouthStartX -= 4;
            mouthEndX += 4;
            mouthStartY = mouthEndY = centreY + 6;
            break;
        case LEFT:
            eyeX -= 3;
            eyeY += 3;
            mouthStartY -= 4;
            mouthEndY += 4;
            mouthStartX = mouthEndX = centreX - 6;
            break;
        case RIGHT:
            eyeX += 3;
            eyeY -= 3;
            mouthStartY -= 4;
            mouthEndY += 4;
            mouthStartX = mouthEndX = centreX + 6;
            break;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_FRect eye = {static_cast<float>(eyeX - 1),
        static_cast<float>(eyeY - 1), 3.0f, 3.0f};
    SDL_RenderFillRect(renderer, &eye);
    SDL_RenderLine(renderer, mouthStartX, mouthStartY, mouthEndX, mouthEndY);
}


void Game::DrawCircle(int32_t game_x, int32_t game_y,
                        bool filled, int32_t diameter)
{

    int32_t centreX =  game_x*TILE_SIZE + TILE_SIZE/2;
    int32_t centreY =  game_y*TILE_SIZE + TILE_SIZE/2;

    int32_t x = (diameter / 2 - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y)
    {
        if (filled)
        {
            SDL_RenderLine(renderer, centreX - x, centreY - y,
                    centreX + x, centreY - y);
            SDL_RenderLine(renderer, centreX - x, centreY + y,
                    centreX + x, centreY + y);
            SDL_RenderLine(renderer, centreX - y, centreY - x,
                    centreX + y, centreY - x);
            SDL_RenderLine(renderer, centreX - y, centreY + x,
                    centreX + y, centreY + x);
        }
        else
        {
            // Each of the following renders an octant of the circle.
            SDL_RenderPoint(renderer, centreX + x, centreY - y);
            SDL_RenderPoint(renderer, centreX + x, centreY + y);
            SDL_RenderPoint(renderer, centreX - x, centreY - y);
            SDL_RenderPoint(renderer, centreX - x, centreY + y);
            SDL_RenderPoint(renderer, centreX + y, centreY - x);
            SDL_RenderPoint(renderer, centreX + y, centreY + x);
            SDL_RenderPoint(renderer, centreX - y, centreY - x);
            SDL_RenderPoint(renderer, centreX - y, centreY + x);
        }

        if (error <= 0)
        {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0)
        {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

SDL_AppResult Game::AppInit() {


    SDL_Color color = { 255, 255, 255, SDL_ALPHA_OPAQUE };
    SDL_Surface *text;

    if(!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }


    if(!SDL_CreateWindowAndRenderer("Snake", WINDOW_WIDTH, WINDOW_HEIGHT, 0,
                &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize SDL_ttf: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    std::filesystem::path fullPath = fontDirectory;
    fullPath /= fontBitCountGridDouble;

    std::string fontPath = fullPath.string();
    font = TTF_OpenFont(fontPath.c_str(), 22);

    if (!font) {
        SDL_Log("Couldn't open font: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }


    // setup static textures
    //
    text = TTF_RenderText_Blended(font, "Game Paused.  SPACE to continue.", 0, color); 
    if (text) {
        textureGamePaused = SDL_CreateTextureFromSurface(renderer, text);
        SDL_DestroySurface(text);
    }
    if (!textureGamePaused) {
        SDL_Log("Couldn't create text: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    text = TTF_RenderText_Blended(font, "Game Over.  SPACE to New Game.", 0, color); 
    if (text) {
        textureYouLost = SDL_CreateTextureFromSurface(renderer, text);
        SDL_DestroySurface(text);
    }
    if (!textureYouLost) {
        SDL_Log("Couldn't create text: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    text = TTF_RenderText_Blended(font, "You Won!  SPACE to New Game.", 0, color); 
    if (text) {
        textureYouWon = SDL_CreateTextureFromSurface(renderer, text);
        SDL_DestroySurface(text);
    }
    if (!textureYouWon) {
        SDL_Log("Couldn't create text: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }


    drawRect.w = drawRect.h = TILE_SIZE;
    srand(time(NULL));

    resetGame();

    ComposeScoreboardText();

    return SDL_APP_CONTINUE;

}

SDL_AppResult Game::handleAppEvent(void *, SDL_Event *event) {

    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_KEY_DOWN:
            switch (event->key.scancode) {
                case SDL_SCANCODE_ESCAPE:
                case SDL_SCANCODE_Q:
                    return SDL_APP_SUCCESS;
                case SDL_SCANCODE_SPACE:
                    if (isGameLost) {
                        isGameLost = false;
                        resetGame();
                    }
                    else {
                        isPaused = !isPaused;
                    }
                    break;
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    mSnake.SetDirection(UP);
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    mSnake.SetDirection(DOWN);
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    mSnake.SetDirection(LEFT);
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    mSnake.SetDirection(RIGHT);
                    break;
                case SDL_SCANCODE_E:
                    resetGame();
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult Game::Iterate(void *) {

    if (!isPaused && !isGameLost) {
        // food logic...is snake head on food?
        SDL_Point mSeg = mSnake.Segment(0);
        int xi = mFood.ContainsPoint(mSeg);
        if (xi >= 0) {
            mSnake.addToSnake(mSnake.segments[mSnake.Size()-1]);
            score++;
            ComposeScoreboardText();
            replaceFood(static_cast<size_t>(xi));
        }

        if (!mSnake.MoveSnake(BOARD_WIDTH_TILES,BOARD_HEIGHT_TILES)) {
            handleGameLost();
        }

    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

            // draw food
            SDL_Point mTmp;
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
            for(size_t i = 0; i < mFood.Size(); i++) {
                mTmp = mFood.FoodStorage(i);
                drawRect.x = mTmp.x * TILE_SIZE;
                drawRect.y = mTmp.y * TILE_SIZE;
                SDL_RenderFillRect(renderer, &drawRect);
            }

            RenderSnake();


            if (isPaused) {
                ShowPaused();
            }
            if (isGameLost) {
                ShowYouLost();
            }

            RenderScoreboard();

            SDL_RenderPresent(renderer);

            // if(mSnake.Size() >= (WINDOW_WIDTH / TILE_SIZE) * (WINDOW_HEIGHT / TILE_SIZE)) {
            if(mSnake.Size() >= 200) {
                printf("You won!\n");
                resetGame();
            }
            SDL_Delay(90);
            return SDL_APP_CONTINUE;
}

