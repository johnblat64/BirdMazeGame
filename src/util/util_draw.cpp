#include "SDL2/include/SDL.h"
#include "util_error_handling.h"
#include "src/global.h"
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
    DrawGrid(SDL_Renderer *renderer, Uint32 spacing, float start_x, float start_y, Uint32 n_rows, Uint32 n_cols,
             SDL_Color color)
    {
        SDLErrorHandle(SDL_SetRenderDrawColor(Global::renderer, color.r, color.g, color.b, color.a));

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

    //--------------------------------------------------------
    void DrawCircleFill(SDL_Renderer *renderer, Uint32 center_x, Uint32 center_y, Uint32 radius, SDL_Color color)
    {
        SDLErrorHandle(SDL_SetRenderDrawColor(Global::renderer, color.r, color.g, color.b, color.a));

        int x = 0;
        int y = radius;
        int d = 3 - 2 * radius;

        SDL_RenderDrawLine(renderer, center_x + x , center_y + y, center_x - x , center_y + y  );
        SDL_RenderDrawLine(renderer, center_x + x , center_y - y, center_x - x , center_y - y );
        SDL_RenderDrawLine(renderer, center_x + y , center_y + x, center_x - y , center_y + x);
        SDL_RenderDrawLine(renderer, center_x + y , center_y - x, center_x - y , center_y - x);

        while( y >= x ) {
            x++;
            if( d > 0 ) {
                y--;
                d = d + 4 * ( x - y ) + 10;
            }
            else {
                d = d + 4 * x + 6;
            }
            SDL_RenderDrawLine(renderer, center_x + x , center_y + y, center_x - x , center_y + y);
            SDL_RenderDrawLine(renderer, center_x + x , center_y - y, center_x - x , center_y - y );
            SDL_RenderDrawLine(renderer, center_x + y , center_y + x , center_x - y , center_y + x);
            SDL_RenderDrawLine(renderer, center_x + y , center_y - x, center_x - y , center_y - x );
        }
    }

    //--------------------------------------------------------
    void 
    RenderInfiniteAxis(SDL_Renderer *renderer, float screen_height, float screen_width, float pan_x, float pan_y, SDL_Color color)
    {
        SDLErrorHandle(SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a));
        // Draw Vertical Line
        SDLErrorHandle(SDL_RenderDrawLine(renderer, pan_x, screen_height, pan_x, 0));

        // Draw Horizontal Line
        SDLErrorHandle(SDL_RenderDrawLine(renderer, 0, pan_y, screen_width, pan_y));
    }
}