#ifndef BIRDMAZEGAME_SPRITE_SHEET_H
#define BIRDMAZEGAME_SPRITE_SHEET_H

#include <external/SDL2/include/SDL.h>
#include <vector>
#include <map>

struct SpriteSheet
{
    SDL_Texture *texture;
    float texture_w;
    float texture_h;
    size_t n_rows;
    size_t n_cols;

    float cell_width()
    {
        return texture_w / n_cols;
    }

    float cell_height()
    {
        return texture_h / n_rows;
    }
};


struct SpriteSheetBitmask
{
    const char *sprite_sheet_name;
    std::vector<Uint8> bitmasks;
};

static std::map<std::string, SpriteSheet> sprite_sheets;


SpriteSheet SpriteSheetCreateFromFile(const char *filename, const char *sprite_sheet_name, int rows, int cols);
SpriteSheet sprite_sheet_get_by_name(std::string sprite_sheet_name);
void SpriteRender(SpriteSheet sprite_sheet, int sprite_sheet_row, int sprite_sheet_col, float pos_x, float pos_y);

#endif