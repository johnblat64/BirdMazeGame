#pragma once

#include <external/SDL2/include/SDL.h>



namespace Util
{
    SDL_Texture *
    Texture_Create_Blank(int width, int height);

    void
    RenderTargetSet(SDL_Renderer *renderer, SDL_Texture *texture);

    void
    DrawGrid(SDL_Renderer *renderer, Uint32 spacing, float start_x, float start_y, Uint32 n_rows, Uint32 n_cols);
}