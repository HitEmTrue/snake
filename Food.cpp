#include "Food.h"

#include <cstdlib>

Food::Food()
    : size(0),
      capacity(0),
      foodStorage(nullptr)
{
}

Food::~Food() {
    free(foodStorage);
}

void Food::Reset() {
    size = 0;
}

size_t Food::Size() {
    return size;
}

const SDL_Point& Food::FoodStorage(size_t index) const {
    return foodStorage[index];
}

int Food::ContainsPoint(SDL_Point point) {
    size_t rValue = -1;
    if (size > 0) {
        for (size_t i = 0; i < size; i++) {
            if ((foodStorage[i].x == point.x) && (foodStorage[i].y == point.y)) {
                rValue = i;
            }
        }
    }
    return rValue;
}

void Food::addToFood(SDL_Point point) {
    if (size == capacity) {
        size_t newCap = (capacity == 0) ? 1 : capacity * 2;
        SDL_Point *tmp_food =
            (SDL_Point *)realloc(foodStorage, newCap * sizeof(SDL_Point));
        if (tmp_food == NULL) {
            exit(1);
        }
        foodStorage = tmp_food;
        capacity = newCap;
    }
    foodStorage[size] = point;
    size++;
}


