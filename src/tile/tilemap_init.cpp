//
// Created by frog on 8/23/22.
//

#include "tilemap.h"


Tilemap
Tilemap_init(Uint32 tile_size, Uint32 n_rows, Uint32 n_cols)
{
    Tilemap tilemap;
    tilemap.tile_size = tile_size;
    tilemap.n_rows = n_rows;
    tilemap.n_cols = n_cols;

    for(int i = 0; i < tilemap.n_rows * tilemap.n_cols; i++)
    {
        tilemap.tileset_sprite_sheet_indices.push_back(TILE_MAP_NO_SPRITE);
        tilemap.is_collision_tiles.push_back(false);
    }

    return tilemap;

}