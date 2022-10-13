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
    Bitmask_Get_Element(int row, int col)
    {
        return bitmasks[row * sprite_sheet.n_cols + col];
    }

    void 
    Bitmask_Set_Element(Uint8 value, int row, int col)
    {
        bitmasks[row * sprite_sheet.n_cols + col] = value;
    }

};

void Tileset_Set_BitMask_Tile(Tileset &tileset, int mouse_x, int mouse_y);

void Tileset_Unset_BitMask_Tile(Tileset &tileset, int mouse_x, int mouse_y);

bool Tileset_Init(SDL_Renderer *renderer, Tileset &working_tileset, std::string file_path, int rows, int cols);

void Bitmask_Render(SDL_Renderer *renderer, Tileset tileset);

void RenderTileset(SDL_Renderer *renderer, Tileset tileset, int x, int y);

void Tileset_Resize_and_Shift_Values(Tileset &tileset, Uint32 new_n_rows, Uint32 new_n_cols);