#pragma once

#include <SDL2/SDL.h>
#include <src/util/util_misc.h>


//--------------------------------
struct WorldPosition  
{ 
    union 
    {
        v2d v2d_val;
        struct 
        {
            float x, y;
        };
    };
};
    



//--------------------------------
struct CollisionRect 
{
    SDL_Rect val;
};

//--------------------------------
struct RotationDegrees
{
    float val;
};



//--------------------------------
typedef enum ButtonState
{
    BUTTON_STATE_JUST_PRESSED,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_NOT_PRESSED,
    BUTTON_STATE_JUST_RELEASED,
} ButtonState;

