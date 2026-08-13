#include <SDL3/SDL_rect.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <format>

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

using namespace std;

#include "Snake.h"
#include "Game.h"

Snake::Snake(Game& game) 
    : size(0),
    direction(1),
    capacity(0),
    mGame(game),
    segments(nullptr)
{
}

Snake::~Snake() {
    free(segments);
}


void Snake::Reset(SDL_Point point) {
    size = 0;
    direction = rand()%4;
    addToSnake(point);
}

int Snake::GetDirection() {
    return direction;
}

size_t Snake::Size() {
    return size;
}

const SDL_Point& Snake::Segment(size_t index) {
    return segments[index];
}

void Snake::SetDirection(int newDirection) {
    direction = newDirection;
}

void Snake::addToSnake(SDL_Point point) {
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


bool Snake::MoveSnake() {


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
        if(segments[0].x >= (mGame.BoardWidth())) segments[0].x = 0;
        if(segments[0].y >= (mGame.BoardHeight())) segments[0].y = 0;
        if(segments[0].x < 0) segments[0].x = (mGame.BoardWidth()) - 1;
        if(segments[0].y < 0) segments[0].y = (mGame.BoardHeight()) - 1;

        for(size_t i = 1; i < size; i++) {
            if((segments[0].x == segments[i].x) && (segments[0].y == segments[i].y)) {
                isAlive = false;
                break;
            }
        }

    }
    return isAlive;
}


bool Snake::ContainsPoint(SDL_Point point ) {
    bool rValue = false;
    if (size > 0) {
        for(size_t i = 0; i < size; i++) {
            if((segments[i].x == point.x) && (segments[i].y == point.y)) {
                rValue = true;
                printf("food on snake prevented\n");
            }
        }
    }
    return rValue;
}

