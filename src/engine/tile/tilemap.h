#ifndef BIRDMAZEGAME_TILEMAP_H
#define BIRDMAZEGAME_TILEMAP_H

#include <stddef.h>
#include <vector>
#include <string>
#include "../sprite/sprite_sheet.h"
#include <external/json/single_include/nlohmann/json.hpp>


const int TILE_MAP_NO_SPRITE = -1;

// define max allowed to prevent user from completely  crashing everything with really high values
// 100 is arbitrary cuz I don't feel like we'll be having maps bigger than that by a long shot

#define MAX_ROWS_ALLOWED 100
#define MAX_COLS_ALLOWED 100


struct Tilemap
{
    std::vector<int> tileset_sprite_sheet_indices;
    std::vector<char> is_collision_tiles; // used to determine auto tiling for the tileset_sprite_sheet_indices
    Uint32 tile_size;
    Uint32 n_rows;
    Uint32 n_cols;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Tilemap, tileset_sprite_sheet_indices, is_collision_tiles, tile_size, n_rows, n_cols)
};

int
two_dim_to_one_dim_index(int row, int col, int n_cols);

Tilemap
Tilemap_init(Uint32 tile_size, Uint32 n_rows, Uint32 n_cols);

void
Tilemap_resize_and_shift_values(Tilemap *tilemap, Uint32 new_n_rows, Uint32 new_n_cols);

void
stdvector_2d_set(std::vector<bool> &v, int row, int col, int n_cols, int value);

int
stdvector_at_2d(std::vector<bool> v, int row, int col, int n_cols);

bool
Tilemap_get_wall_value(Tilemap tilemap, int row, int col);

void
Tilemap_set_collision_tile_value(Tilemap *tilemap, int row, int col, bool is_collision_tile);


template <typename T>void
stdvector_2d_set(std::vector<T> &v, int row, int col, int n_cols, int value)
{
    int index = two_dim_to_one_dim_index(row, col, n_cols);
    v[index] = value;
}

template <typename T> T
stdvector_at_2d(std::vector<T> v, int row, int col, int n_cols)
{
    int index = two_dim_to_one_dim_index(row, col, n_cols);
    return v[index];
}

void
TilemapCollisionTileRectsRender(Tilemap &tilemap, float pos_x, float pos_y, SDL_Color color);

void
TilemapGridRender(Tilemap &tilemap, float pos_x, float pos_y, SDL_Color color);

#endif