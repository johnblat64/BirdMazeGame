#pragma once
#include <external/SDL2/include/SDL.h>
#include <stdio.h>
#include "src/engine/sprite/sprite_sheet.h"
#include <string>

struct Tileset 
{
    SpriteSheet sprite_sheet;
    std::vector<Uint8> bitmasks;

    Uint8
    bitmask_get_element(int row, int col)
    {
        return bitmasks[row * sprite_sheet.n_cols + col];
    }

    void 
    bitmask_set_element(Uint8 value, int row, int col)
    {
        bitmasks[row * sprite_sheet.n_cols + col] = value;
    }

};

void TilesetSetBitMaskTile(Tileset &tileset, int mouse_x, int mouse_y);

void TilesetUnsetBitMaskTile(Tileset &tileset, int mouse_x, int mouse_y);

bool TilesetInit(SDL_Renderer *renderer, Tileset &working_tileset, std::string file_path, int rows, int cols);

void BitmaskRender(SDL_Renderer *renderer, Tileset tileset);

void RenderTileset(SDL_Renderer *renderer, Tileset tileset, int x, int y);

void TilesetResizeandShiftValues(Tileset &tileset, Uint32 new_n_rows, Uint32 new_n_cols);