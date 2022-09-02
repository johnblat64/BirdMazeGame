#include <stddef.h>
#include <vector>
#include <string>
#include "../sprite/sprite_sheet.h"

const int TILE_MAP_NO_SPRITE = -1;

// define max allowed to prevent user from completely  crashing everything with really high values
// 100 is arbitrary cuz I don't feel like we'll be having maps bigger than that by a long shot

#define MAX_ROWS_ALLOWED 100
#define MAX_COLS_ALLOWED 100


struct AutoTiledTileMap
{
    std::string tileset_sprite_sheet_name;
    std::vector<Uint8> auto_tile_tileset_bitmasks;
    std::vector<int> tileset_sprite_sheet_indices;
    std::vector<bool> walls; // used to determine auto tiling for the tileset_sprite_sheet_indices
    Uint16 tile_size;
    Uint32 n_rows;
    Uint32 n_cols;
};


AutoTiledTileMap
AutoTiledTileMap_init(const char *sprite_sheet_name, Uint32 tile_size, Uint32 n_rows, Uint32 n_cols);

void
AutoTiledTileMap_resize_and_shift_values(AutoTiledTileMap *tilemap, Uint32 new_n_rows, Uint32 new_n_cols);

void
set_2d(std::vector<bool> &v, int row, int col, int n_cols, int value);

int
at_2d(std::vector<bool> v, int row, int col, int n_cols);

bool 
AutoTiledTileMap_get_wall_value(AutoTiledTileMap tilemap, int row, int col);

void 
AutoTiledTileMap_set_wall_value(AutoTiledTileMap *tilemap, int row, int col, bool wall_value);