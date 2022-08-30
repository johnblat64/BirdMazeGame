#include <SDL2/SDL.h>


void 
util_draw_grid(SDL_Renderer *renderer, int spacing, float x, float y, int n_rows, int n_cols) 
{
    int h = n_rows * spacing;
    int w = n_cols * spacing;

    int end_x = x + w;
    int end_y = y + h;

    for(float xi = x; xi <= end_x; xi += spacing) 
    {
        SDL_RenderDrawLineF(renderer, xi, y, xi, end_y);
    }

    for(float yi = y; yi <= end_y; yi += spacing) 
    {
        SDL_RenderDrawLineF(renderer, x, yi, end_x, yi);
    }
}