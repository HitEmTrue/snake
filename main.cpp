#include <SDL3/SDL_init.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3_ttf/SDL_textengine.h>
#include <SDL3_ttf/SDL_ttf.h>


#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdlib.h>

#include "Game.h"



SDL_AppResult SDL_AppInit(void **appstate, int, char *[]) {

    *appstate = new Game;
    return static_cast<Game*>(*appstate)->AppInit();

}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {

    return static_cast<Game*>(appstate)->handleAppEvent(appstate, event);
  
}

SDL_AppResult SDL_AppIterate(void *appstate) {

    return static_cast<Game*>(appstate)->Iterate(appstate);

}

void SDL_AppQuit(void *appstate, SDL_AppResult) {

    delete static_cast<Game*>(appstate);
}
