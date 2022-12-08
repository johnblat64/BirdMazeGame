#include "util_load_save.h"
#include "util_error_handling.h"
#include <stdio.h>

bool SDLLogCall(const char* function, const char* file, int line, int flag)
{
    if (flag != 0) {
        const char* error = SDL_GetError();
        printf("[SDL Error] %s %s %s : line %i\n", error, function, file, line);
        return false;
    }
    return true;
}


void LogErrorMessage(const char *message, const char *file, int line)
{
    printf("Error %s  %s : line %i\n", message, file, line);
}
