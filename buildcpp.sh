g++ -std=c++20 -Wall -Wextra main.cpp Game.cpp Snake.cpp Food.cpp \
    $(pkg-config --cflags --libs sdl3 sdl3-ttf) \
    -o snake
