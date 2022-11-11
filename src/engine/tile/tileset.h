#pragma once
#include <external/SDL2/include/SDL.h>
#include <stdio.h>
#include "src/engine/sprite/sprite_sheet.h"
#include <string>
#include <external/json/single_include/nlohmann/json.hpp>

struct Tileset 
{
    SpriteSheet sprite_sheet;
    std::vector<Uint8> bitmasks;
    std::string file_name;
    bool texture_initialized = false;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Tileset, sprite_sheet, bitmasks, file_name)


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

bool TilesetCreateNewTextureandTileset(SDL_Renderer *renderer, Tileset &working_tileset, std::string file_path, int rows, int cols);

bool TilesetLoadTilesetTexture(SDL_Renderer *renderer, Tileset &tileset, std::string file_path);

void BitmaskRender(SDL_Renderer *renderer, Tileset tileset);

void RenderTileset(SDL_Renderer *renderer, Tileset tileset, int x, int y);

void TilesetResizeandShiftValues(Tileset &tileset, Uint32 new_n_rows, Uint32 new_n_cols);

Tileset TilesetInit(int rows, int cols);