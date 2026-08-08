#include <SDL3/SDL_init.h>
#include <SDL3/SDL_pixels.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define TILE_SIZE 20

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3



class Game {
    bool grow;
    int wins;
    int losses;
    int direction;
    size_t snakeSize, snakeCap, foodSize, foodCap;
    SDL_Point *snake, *food;
    SDL_FRect drawRect;

    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Point randomTile() {
        return (SDL_Point){rand() % (WINDOW_WIDTH / TILE_SIZE - 1),
               rand() % (WINDOW_HEIGHT / TILE_SIZE - 1)};
    }
    
    public:
        Game()
            : renderer (nullptr),
                window (nullptr),
                snakeSize(0)
        {
        }

        void resetGame() {
          snakeSize = foodSize = 0;
          addToSnake(randomTile());
          for(int f = 0; f < (rand() % 4) + 3; f++) addToFood(randomTile());
          grow = false;
          direction = (rand() % 4);
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


        void DrawSnakeBodySegment(int32_t game_x, int32_t game_y) {
            SDL_SetRenderDrawColor(renderer, 180, 50, 50, SDL_ALPHA_OPAQUE);
            DrawCircle(renderer, game_x, game_y, true, TILE_SIZE - 4);
        }

        void DrawSnakeHead(int32_t game_x, int32_t game_y) {
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, SDL_ALPHA_OPAQUE);
            DrawCircle(renderer, game_x, game_y, true, TILE_SIZE);

            const int32_t centreX = game_x * TILE_SIZE + TILE_SIZE / 2;
            const int32_t centreY = game_y * TILE_SIZE + TILE_SIZE / 2;
            int32_t eyeX = centreX;
            int32_t eyeY = centreY;
            int32_t mouthStartX = centreX;
            int32_t mouthStartY = centreY;
            int32_t mouthEndX = centreX;
            int32_t mouthEndY = centreY;

            switch (direction) {
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



        void DrawCircle(SDL_Renderer * renderer, int32_t game_x, int32_t game_y,
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


        void addToSnake(SDL_Point point) {
          if(snakeSize == snakeCap) {
            size_t newCap = (snakeCap == 0) ? 1 : snakeCap * 2;
            SDL_Point *tmp_snake = (SDL_Point *)realloc(snake, newCap * sizeof(SDL_Point));
            if(tmp_snake == NULL) {
              free(snake);
              free(food);
              exit(1);
            }
            snake = tmp_snake;
            snakeCap = newCap;
          }
          snake[snakeSize] = point;
          snakeSize++;
        }

        SDL_AppResult AppInit(void **appstate) {

            Game *game = static_cast<Game *>(*appstate);

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

              drawRect.w = drawRect.h = TILE_SIZE;
              srand(time(NULL));

              game->resetGame();

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
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                          direction = UP;
                          break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                          direction = DOWN;
                          break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                          direction = LEFT;
                          break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                          direction = RIGHT;
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
        
          SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
          SDL_RenderClear(renderer);

          SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
          for(size_t i = 0; i < foodSize; i++) {
            drawRect.x = food[i].x * TILE_SIZE;
            drawRect.y = food[i].y * TILE_SIZE;
            SDL_RenderFillRect(renderer, &drawRect);
            if((snakeSize > 0) && (food[i].x == snake[0].x) &&
                (food[i].y == snake[0].y)) {
              grow = true;
              food[i] = randomTile();
            }
          }

          if(grow) game->addToSnake(snake[snakeSize - 1]);
          grow = false;

          for(size_t i = snakeSize; i > 1; i--) {
            snake[i - 1] = snake[i - 2];
          }

            // draw snake body
          for(size_t i = 1; i < snakeSize; i++) {
            DrawSnakeBodySegment(snake[i].x, snake[i].y);
          }

          if(snakeSize > 0) {
            if(direction == UP) snake[0].y--;
            if(direction == DOWN) snake[0].y++;
            if(direction == LEFT) snake[0].x--;
            if(direction == RIGHT) snake[0].x++;
            if(snake[0].x >= (WINDOW_WIDTH / TILE_SIZE)) snake[0].x = 0;
            if(snake[0].y >= (WINDOW_HEIGHT / TILE_SIZE)) snake[0].y = 0;
            if(snake[0].x < 0) snake[0].x = (WINDOW_WIDTH / TILE_SIZE) - 1;
            if(snake[0].y < 0) snake[0].y = (WINDOW_HEIGHT / TILE_SIZE) - 1;

            for(size_t i = 1; i < snakeSize; i++) {
              if((snake[0].x == snake[i].x) && (snake[0].y == snake[i].y)) {
                printf("You died.\n");
                game->resetGame();
                break;
              }
            }

            // draw snake head
            DrawSnakeHead(snake[0].x, snake[0].y);
          }

          SDL_RenderPresent(renderer);

          // if(snakeSize >= (WINDOW_WIDTH / TILE_SIZE) * (WINDOW_HEIGHT / TILE_SIZE)) {
          if(snakeSize >= 40) {
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
