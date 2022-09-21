#include <external/SDL2/include/SDL.h>
#include <src/engine/util/util_error_handling.h>
#include <src/engine/global.h>
#include "util_draw.h"

namespace Util
{
    //--------------------------------------------------------
    SDL_Texture *
    Texture_Create_Blank(int width, int height)
    {
        SDL_Texture *texture;
        texture = SDL_CreateTexture(Global::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
        SDLErrorHandleNull(texture);

        return texture;
    }


    //--------------------------------------------------------
    void
    RenderTargetSet(SDL_Renderer *renderer, SDL_Texture *texture)
    {
        SDLErrorHandle(SDL_SetRenderTarget(renderer, texture));
    }


    //--------------------------------------------------------
    void
    DrawGrid(SDL_Renderer *renderer, Uint32 spacing, float start_x, float start_y, Uint32 n_rows, Uint32 n_cols)
    {
        Uint32 grid_h = n_rows * spacing;
        Uint32 grid_w = n_cols * spacing;

        float end_x = start_x + static_cast<float>(grid_w);
        float end_y = start_y + static_cast<float>(grid_h);

        for(Uint32 col = 0; col <= n_cols; col++) // <=  so that it draws the rightmost border
        {
            float x = start_x + static_cast<float>(col * spacing);
            SDL_RenderDrawLineF(renderer, x, start_y, x, end_y);

        }

        for(Uint32 row = 0; row <= n_rows; row++) // <=  so that it draws the rightmost border
        {
            float y = start_y + static_cast<float>(row * spacing);
            SDL_RenderDrawLineF(renderer, start_x, y, end_x, y);

        }
    }
}