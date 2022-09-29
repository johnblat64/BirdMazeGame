#pragma once

#include <SDL2/SDL.h>

//--------------------------------
struct v2d
{
    float x, y;
};

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
struct Velocity
{
    v2d val;
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

struct MouseState
{
    v2d window_mouse_pos;
    v2d logical_mouse_pos; // based on render scale
    ButtonState left_mouse_btn_state; 
    ButtonState right_mouse_btn_state; 
    v2d scroll_movement;
};

//--------------------------------
