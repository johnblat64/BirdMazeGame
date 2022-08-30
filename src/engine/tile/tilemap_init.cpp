//
// Created by frog on 8/23/22.
//

#include "tilemap.h"


AutoTiledTileMap
AutoTiledTileMap_init(const char *sprite_sheet_name, Uint32 tile_size, Uint32 n_rows, Uint32 n_cols)
{
    AutoTiledTileMap tilemap;
    tilemap.tileset_sprite_sheet_name = sprite_sheet_name;
    tilemap.tile_size = tile_size;
    tilemap.n_rows = n_rows;
    tilemap.n_cols = n_cols;

    for(int i = 0; i < tilemap.n_rows * tilemap.n_cols; i++)
    {
        tilemap.tileset_sprite_sheet_indices.push_back(TILE_MAP_NO_SPRITE);
        tilemap.walls.push_back(false);
    }

    return tilemap;

}