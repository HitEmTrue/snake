#pragma once

#include <cstddef>
#include <SDL3/SDL_rect.h>

class Food {
    size_t size;
    size_t capacity;

    public:
        Food();

        ~Food();

        SDL_Point *foodStorage;

        void Reset();

        size_t Size();

        const SDL_Point& FoodStorage(size_t index) const;
        


        int ContainsPoint(SDL_Point point );

        void addToFood(SDL_Point point);
};
