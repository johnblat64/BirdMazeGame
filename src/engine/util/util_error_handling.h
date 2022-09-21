#ifndef BIRDMAZEGAME_UTIL_ERROR_HANDLING_H
#define BIRDMAZEGAME_UTIL_ERROR_HANDLING_H

#ifndef NDEBUG
#ifndef MSVC
#include <signal.h>
#define DEBUG_BREAK raise(SIGTRAP)
#else
#define DEBUG_BREAK __debugbreak()
#endif                                      // MSVC
#else
#define DEBUG_BREAK 0
#endif                                          // DEBUG

#include <assert.h>
#include <SDL.h>

// breaks into debug mode on false value. 
#define ASSERT(x) if (!(x)) DEBUG_BREAK;

// executes function saves its success flags and checks it against our assert
#define SDLErrorHandle(x)                                          \
    {                                                       \
        SDL_ClearError();                                   \
        int flag = x;                                       \
        ASSERT(SDLLogCall(#x, __FILE__, __LINE__, flag))    \
    }

// for SDL function that return null as opposed to integer flags 
#define SDLErrorHandleNull(x)                                      \
{                                                           \
   int flag = -1 ? x == NULL : 0;                           \
   ASSERT(SDLLogCall(#x, __FILE__, __LINE__, flag))         \
}

#define ErrorLog(message) \
{                         \
    LogErrorMessage(message, __FILE__, __LINE__);\
}

bool SDLLogCall(const char* function, const char* file, int line, int flag);
void LogErrorMessage(const char *message, const char *file, int line);

#endif //BIRDMAZEGAME_UTIL_ERROR_HANDLING_H