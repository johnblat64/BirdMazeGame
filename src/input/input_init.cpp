#include <SDL2/SDL.h>
#include <vector>
#include "src/components/components.h"

struct Input
{
    Uint8 *key_states;
    std::vector<SDL_GameController> sdl_game_controllers;
    v2d window_mouse_pos;
    v2d logical_scaled_mouse_pos; // scaled with the renderer
    v2d mouse_scroll_movement;
};

Input input;


Uint8 *Input_key_states_get()
{
    return input.key_states;
}
