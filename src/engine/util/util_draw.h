#pragma once

#include <external/SDL2/include/SDL.h>


namespace Util
{
    SDL_Texture *
    Texture_Create_Blank(int width, int height);

    void
    RenderTargetSet(SDL_Renderer *renderer, SDL_Texture *texture);

    void
    DrawGrid(SDL_Renderer *renderer, Uint32 spacing, float start_x, float start_y, Uint32 n_rows, Uint32 n_cols,
             SDL_Color color);

    void
    DrawCircleFill(SDL_Renderer *renderer, Uint32 center_x, Uint32 center_y, Uint32 radius, SDL_Color color);

    void
    RenderInfiniteAxis(SDL_Renderer *renderer, float screen_height, float screen_width, float pan_x, float pan_y, SDL_Color color);

}