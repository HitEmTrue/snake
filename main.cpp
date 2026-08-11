#include <SDL3/SDL_init.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3_ttf/SDL_textengine.h>
#include <SDL3_ttf/SDL_ttf.h>


#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <filesystem>
#include <format>

using namespace std;


//#define WINDOW_WIDTH 640
//#define WINDOW_HEIGHT 480
//#define TILE_SIZE 20

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3


class Snake {
    size_t size;
    int direction;
    size_t capacity;

    public:
        Snake() 
            : size(0),
            direction(1),
            capacity(0),
            segments(nullptr)
        {
        }

        SDL_Point *segments;

        void Reset(SDL_Point point) {
            size = 0;
            direction = rand()%4;
            std::string debug = std::format("pointx: {}    Pointy: {}\n", point.x, point.y);
            printf("%s", debug.c_str());
            addToSnake(point);
            debug = std::format("Snake size: {} segments[0].x: {} segments[0].y: {} direction: {}\n", 
                    size, segments[0].x, segments[0].y, direction);
            printf("%s", debug.c_str());
        }

        int GetDirection() {
            return direction;
        }

        size_t Size() {
            return size;
        }

        void SetDirection(int newDirection) {
            direction = newDirection;
        }

        // mSnake.addToSnake(snake[snakeSize - 1]);
        

        // void addToSnake(SDL_Point point) {
        void addToSnake(SDL_Point point) {
            if(size == capacity)  {
                size_t newCap = (capacity == 0) ? 1 : capacity * 2;
                SDL_Point *tmp_snake = (SDL_Point *)realloc(segments, newCap * sizeof(SDL_Point));
                if(tmp_snake == NULL) {
                    // get out of here gracefully
                    exit(1);
                }
                segments = tmp_snake;
                capacity = newCap;
            }
          segments[size] = point;
          size++;


        }


        bool MoveSnake( int boardWidth, int boardHeight) {


            bool isAlive = true;

            std::string debug = std::format("SnakeMove() size: {} segments[0].x: {} segments[0].y: {} direction: {}\n", 
                    size, segments[0].x, segments[0].y, direction);
            // printf("%s", debug.c_str());
            // calculate new positions for snake body
            // this is follow the leader
            for(size_t i = size; i > 1; i--) {
                segments[i - 1] = segments[i - 2];
            }

            // calculate new position for snake head
            if(size > 0) {
                if(direction == UP) segments[0].y--;
                if(direction == DOWN) segments[0].y++;
                if(direction == LEFT) segments[0].x--;
                if(direction == RIGHT) segments[0].x++;
                if(segments[0].x >= (boardWidth)) segments[0].x = 0;
                if(segments[0].y >= (boardHeight)) segments[0].y = 0;
                if(segments[0].x < 0) segments[0].x = (boardWidth) - 1;
                if(segments[0].y < 0) segments[0].y = (boardHeight) - 1;

                for(size_t i = 1; i < size; i++) {
                    if((segments[0].x == segments[i].x) && (segments[0].y == segments[i].y)) {
                        isAlive = false;
                        break;
                    }
                }

            }
            return isAlive;
        }
}; 


class Game {
    Snake mSnake;
    bool grow;
    bool isPaused;
    bool isGameLost;
    size_t highScore;
    int losses;
    int direction;
    size_t snakeSize, snakeCap, foodSize, foodCap;
    SDL_Point *snake, *food;
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
        Game()
          : grow(false),
            isPaused(false),
            isGameLost(false),
            highScore(0),
            losses(0),
            direction(UP),
            snakeSize(0),
            snakeCap(0),
            foodSize(0),
            foodCap(0),
            snake(nullptr),
            food(nullptr),
            window(nullptr),
            renderer(nullptr),
            font(nullptr),
            textureGamePaused(nullptr),
            textureYouWon(nullptr),
            textureYouLost(nullptr),
            BOARD_WIDTH_TILES(WINDOW_WIDTH / TILE_SIZE),
            BOARD_HEIGHT_TILES((WINDOW_HEIGHT - SCOREBOARD_HEIGHT) / TILE_SIZE)
        {
        }

        void resetGame() {
            foodSize = 0;
            for(int f = 0; f < (rand() % 4) + 3; f++) addToFood(randomTile());
            mSnake.Reset(randomTile());
        }

        void addToFood(SDL_Point point) {
            if(foodSize == foodCap) {
                size_t newCap = (foodCap == 0) ? 1 : foodCap * 2;
                SDL_Point *tmp_food =
                    (SDL_Point *)realloc(food, newCap * sizeof(SDL_Point));
                if(tmp_food == NULL) {
                    free(snake);
                    free(food);
                    exit(1);
                }
                food = tmp_food;
                foodCap = newCap;
            }
            food[foodSize] = point;
            foodSize++;
        }

        void handleGameLost() {
            isGameLost = true;

            if (mSnake.Size()  > highScore ) {
                highScore = mSnake.Size();
                ComposeScoreboardText();
            }
        }


        void dropFood(size_t foodIndex) {

            bool foodLocationFound = false;
            SDL_Point potentialFood;

            while (!foodLocationFound) {
                bool isCollision = false;

                potentialFood = randomTile();

                for(size_t i = 0; i < foodSize; i++) {
                    if((food[i].x == potentialFood.x) &&
                            (food[i].y == potentialFood.y)) {
                        isCollision = true;
                        printf("food double prevented\n");
                    }
                }

                // no reason to check the snake...find another food candidate
                if (isCollision)
                    continue;

                if (snakeSize > 0) {
                    for(size_t i = 0; i < snakeSize; i++) {
                        if((snake[i].x == potentialFood.x) && (snake[i].y == potentialFood.y)) {
                            isCollision = true;
                            printf("food on snake prevented\n");
                        }
                    }
                }

                if (!isCollision)
                    foodLocationFound = true;
            }

            food[foodIndex] = potentialFood;
        }

        void ComposeScoreboardText() {

            printf("inisde ComposeScoreboardText()\n");
        
            SDL_Color color = { 255, 255, 255, SDL_ALPHA_OPAQUE };
            SDL_Surface *text;
            std::string snakeBanner = std::format("Score : {}                  High score : {}", mSnake.Size()-1, highScore);

            text = TTF_RenderText_Blended(font, snakeBanner.c_str(), 0, color); 
            if (text) {
                textureSnakeLength = SDL_CreateTextureFromSurface(renderer, text);
                SDL_DestroySurface(text);
            }
            if (!textureSnakeLength) {
                SDL_Log("Couldn't create text: %s\n", SDL_GetError());
            }
        }


        void ShowPaused() {
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
        void ShowYouLost() {
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
        
        void RenderScoreboard() {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderLine(renderer, 0, WINDOW_HEIGHT - SCOREBOARD_HEIGHT + 1, WINDOW_WIDTH, WINDOW_HEIGHT - SCOREBOARD_HEIGHT + 1);
            

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


        void DrawSnakeBodySegment(int32_t game_x, int32_t game_y) {
            SDL_SetRenderDrawColor(renderer, 180, 50, 50, SDL_ALPHA_OPAQUE);
            DrawCircle(game_x, game_y, true, TILE_SIZE - 4);
        }

        void DrawSnakeHead(int32_t game_x, int32_t game_y) {
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

        void DrawCircle(int32_t game_x, int32_t game_y,
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



        SDL_AppResult AppInit(void **appstate) {

            Game *game = static_cast<Game *>(*appstate);

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

            printf("Calling resetGame() from Game init()\n");
            game->resetGame();

            ComposeScoreboardText();

            return SDL_APP_CONTINUE;

        }

        SDL_AppResult handleAppEvent(void *appstate, SDL_Event *event) {

            Game *game = static_cast<Game *>(appstate);
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
                            game->resetGame();
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

        SDL_AppResult Iterate(void *appstate) {

            Game *game = static_cast<Game *>(appstate);

            if (!isPaused && !isGameLost) {
                // food logic...is snake head on food?
                for(size_t i = 0; i < foodSize; i++) {
                    if((mSnake.Size() > 0) && (food[i].x == mSnake.segments[0].x) &&
                            (food[i].y == mSnake.segments[0].y)) {
                        mSnake.addToSnake(mSnake.segments[mSnake.Size()-1]);
                      // mSnake.addToSnake(snake[snakeSize - 1]);
                        // grow faster for testing
                        //addToSnake(snake[snakeSize - 1]);
                        ComposeScoreboardText();
                        dropFood(i);
                    }
                }

                if (!mSnake.MoveSnake(BOARD_WIDTH_TILES,BOARD_HEIGHT_TILES)) {
                    handleGameLost();
                }

            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(renderer);

            // draw food
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
            for(size_t i = 0; i < foodSize; i++) {
                drawRect.x = food[i].x * TILE_SIZE;
                drawRect.y = food[i].y * TILE_SIZE;
                SDL_RenderFillRect(renderer, &drawRect);
            }


            if (mSnake.Size() > 0) {
                // draw snake body
                for(size_t i = 1; i < mSnake.Size(); i++) {
                    DrawSnakeBodySegment(mSnake.segments[i].x, mSnake.segments[i].y);
                }
                // draw snake head
                DrawSnakeHead(mSnake.segments[0].x, mSnake.segments[0].y);
            }

            if (isPaused) {
                ShowPaused();
            }
            if (isGameLost) {
                ShowYouLost();
            }

            RenderScoreboard();

            SDL_RenderPresent(renderer);

            // if(snakeSize >= (WINDOW_WIDTH / TILE_SIZE) * (WINDOW_HEIGHT / TILE_SIZE)) {
            if(snakeSize >= 200) {
                printf("You won!\n");
                game->resetGame(); }
            SDL_Delay(90);
            return SDL_APP_CONTINUE;
        }



        void appQuit(void) {

            free(snake);
            free(food);
        }



        };

SDL_AppResult SDL_AppInit(void **appstate, int, char *[]) {

    *appstate = new Game;
    return static_cast<Game*>(*appstate)->AppInit(appstate);

}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {

    return static_cast<Game*>(appstate)->handleAppEvent(appstate, event);
  
}

SDL_AppResult SDL_AppIterate(void *appstate) {

    return static_cast<Game*>(appstate)->Iterate(appstate);

}

void SDL_AppQuit(void *appstate, SDL_AppResult) {

  static_cast<Game*>(appstate)->appQuit();

}
