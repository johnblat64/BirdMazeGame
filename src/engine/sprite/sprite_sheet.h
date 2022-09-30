#ifndef BIRDMAZEGAME_SPRITE_SHEET_H
#define BIRDMAZEGAME_SPRITE_SHEET_H

#include <external/SDL2/include/SDL.h>
#include <vector>
#include <map>
#include "src/engine/tile/tilemap.h"

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

struct AnimatedSprite
{
    SpriteSheet sprite_sheet;
    Uint32 start_frame;
    Uint32 end_frame;
    Uint32 curr_frame;
    double accumulator;
    double FPS;
    bool flip_horizonatally;

    void increment(float delta_time_seconds)
    {
        accumulator += delta_time_seconds;
        if (accumulator >= seconds_per_frame())
        {
            accumulator = 0.0f;
            curr_frame++;

            if (curr_frame >= end_frame)
            {
                curr_frame = start_frame;
            }
        }
    }

    double seconds_per_frame()
    {
        return 1.0f / FPS;
    }
};


AnimatedSprite
AnimatedSpriteInit(SpriteSheet sprite_sheet, Uint32 start_frame, Uint32 end_frame, float FPS);


struct SpriteSheetBitmask
{
    const char *sprite_sheet_name;
    std::vector<Uint8> bitmasks;
};

static std::map<std::string, SpriteSheet> sprite_sheets;


SpriteSheet SpriteSheetCreateFromFile(const char *filename, const char *sprite_sheet_name, int rows, int cols);
SpriteSheet sprite_sheet_get_by_name(std::string sprite_sheet_name);
void SpriteRender(SpriteSheet sprite_sheet, int sprite_sheet_row, int sprite_sheet_col, float scale, float tilemap_x,
                  float tilemap_y, float parent_x, float parent_y, float local_sprite_x, float local_sprite_y,
                  bool flip);

#endif