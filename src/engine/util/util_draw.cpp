#include <SDL2/SDL.h>


void 
util_draw_grid(SDL_Renderer *renderer, Uint32 spacing, float start_x, float start_y, Uint32 n_rows, Uint32 n_cols)
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