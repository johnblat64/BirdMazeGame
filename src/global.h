//
// Created by frog on 9/19/22.
//

#ifndef BIRDMAZEGAMEEDITOR_GLOBAL_H
#define BIRDMAZEGAMEEDITOR_GLOBAL_H
#include "SDL2/include/SDL.h"


namespace Global
{
    extern SDL_Window *window;
    extern SDL_Renderer *renderer;
    extern SDL_Event event;
    extern Uint32 delta_time_ms;
    extern int window_w;
    extern int window_h;

}

#endif //BIRDMAZEGAMEEDITOR_GLOBAL_H
