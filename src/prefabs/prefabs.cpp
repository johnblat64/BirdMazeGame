#include <flecs/flecs.h>
#include "../components/components.h"
#include ""

flecs::entity prefab_EditorUser(flecs::world world)
{
    MouseState mouse_state;
    mouse_state.left_mouse_btn_state = BUTTON_STATE_NOT_PRESSED;
    mouse_state.right_mouse_btn_state = BUTTON_STATE_NOT_PRESSED;
    mouse_state.logical_mouse_pos = (v2d){0.0,0.0};
    mouse_state.window_mouse_pos = (v2d){0.0,0.0};
    mouse_state.scroll_movement = (v2d){0.0,0.0};


    flecs::entity e = world.prefab("EditorUser")
        .set<MouseState>(mouse_state);

    return e;
} 

flecs::entity prefab_AutoTiledTileMap(flecs::world world)
{
    MouseState mouse_state;
    mouse_state.left_mouse_btn_state = BUTTON_STATE_NOT_PRESSED;
    mouse_state.right_mouse_btn_state = BUTTON_STATE_NOT_PRESSED;
    mouse_state.logical_mouse_pos = (v2d){0.0,0.0};
    mouse_state.window_mouse_pos = (v2d){0.0,0.0};
    mouse_state.scroll_movement = (v2d){0.0,0.0};


    flecs::entity e = world.prefab("EditorUser")
        .set<MouseState>(mouse_state);

    return e;
} 