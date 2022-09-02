#ifndef BIRDMAZEGAME_UTIL_ERROR_HANDLING_H
#define BIRDMAZEGAME_UTIL_ERROR_HANDLING_H

#include <assert.h>
#include <SDL.h>

// breaks into debug mode on false value. 
#define ASSERT(x) if (!(x)) __debugbreak();

// executes function 
#define SDLCall(x)                                          \
    {                                                       \
        SDL_ClearError();                                   \
        int flag = x;                                       \
        ASSERT(SDLLogCall(#x, __FILE__, __LINE__, flag))    \
    }

bool SDLLogCall(const char* function, const char* file, int line, int flag);

#endif //BIRDMAZEGAME_UTIL_ERROR_HANDLING_H