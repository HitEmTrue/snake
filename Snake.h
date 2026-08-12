#pragma once

#include <SDL3/SDL_rect.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <filesystem>
#include <format>

using namespace std;


class Snake {
    size_t size;
    int direction;
    size_t capacity;

    public:
        Snake(); 

        ~Snake();

        SDL_Point *segments;

        void Reset(SDL_Point point); 

        int GetDirection();

        size_t Size();

        const SDL_Point& Segment(size_t index);

        void SetDirection(int newDirection);

        void addToSnake(SDL_Point point);

        bool MoveSnake( int boardWidth, int boardHeight);

        bool ContainsPoint(SDL_Point point );
}; 
