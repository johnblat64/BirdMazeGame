#include "../src/engine/tile/tilemap.h"
#include <cassert>

void AutoTiledTileMap_fill_with_dummy_values(AutoTiledTileMap *tilemap)
{
    for(int row = 0; row < tilemap->n_rows; row++)
    {
        for(int col = 0; col < tilemap->n_cols; col++)
        {
            int val = row * 100;
            val += col;
            set_2d(tilemap->tileset_sprite_sheet_indices, row, col, tilemap->n_cols, val);
        }
    }
}


bool AutoTiledTileMap_resize_and_shift_values_add_rows()
{
    AutoTiledTileMap tilemap = AutoTiledTileMap_init("foo", 8, 6);
    AutoTiledTileMap_fill_with_dummy_values(&tilemap);
    AutoTiledTileMap_resize_and_shift_values(&tilemap, 11, 6);

    return true;


}

bool AutoTiledTileMap_resize_and_shift_values_remove_rows()
{
    AutoTiledTileMap tilemap = AutoTiledTileMap_init("foo", 8, 6);
    AutoTiledTileMap_fill_with_dummy_values(&tilemap);

    AutoTiledTileMap_resize_and_shift_values(&tilemap, 5, 6);

    return true;
}

bool AutoTiledTileMap_resize_and_shift_values_add_cols()
{
    AutoTiledTileMap tilemap = AutoTiledTileMap_init("foo", 8, 6);
    AutoTiledTileMap_fill_with_dummy_values(&tilemap);

    AutoTiledTileMap_resize_and_shift_values(&tilemap, 8, 9);

    return true;
}

bool AutoTiledTileMap_resize_and_shift_values_remove_cols()
{
    AutoTiledTileMap tilemap = AutoTiledTileMap_init("foo", 8, 6);
    AutoTiledTileMap_fill_with_dummy_values(&tilemap);

    AutoTiledTileMap_resize_and_shift_values(&tilemap, 8, 3);

    return true;
}

bool AutoTiledTileMap_resize_and_shift_values_add_rows_and_cols()
{
    AutoTiledTileMap tilemap = AutoTiledTileMap_init("foo", 8, 6);
    AutoTiledTileMap_fill_with_dummy_values(&tilemap);

    AutoTiledTileMap_resize_and_shift_values(&tilemap, 11, 9);

    return true;
}


bool AutoTiledTileMap_resize_and_shift_values_add_rows_and_remove_cols()
{
    AutoTiledTileMap tilemap = AutoTiledTileMap_init("foo", 8, 6);
    AutoTiledTileMap_fill_with_dummy_values(&tilemap);

    AutoTiledTileMap_resize_and_shift_values(&tilemap, 11, 3);

    return true;
}

bool AutoTiledTileMap_resize_and_shift_values_remove_rows_and_add_cols()
{
    AutoTiledTileMap tilemap = AutoTiledTileMap_init("foo", 8, 6);
    AutoTiledTileMap_fill_with_dummy_values(&tilemap);

    AutoTiledTileMap_resize_and_shift_values(&tilemap, 5, 9);

    return true;
}


bool AutoTiledTileMap_resize_and_shift_values_remove_rows_and_remove_cols()
{
    AutoTiledTileMap tilemap = AutoTiledTileMap_init("foo", 8, 6);
    AutoTiledTileMap_fill_with_dummy_values(&tilemap);

    AutoTiledTileMap_resize_and_shift_values(&tilemap, 5, 3);

    return true;
}


int main(){
    AutoTiledTileMap_resize_and_shift_values_add_rows();
    AutoTiledTileMap_resize_and_shift_values_remove_rows();
    AutoTiledTileMap_resize_and_shift_values_add_cols();
    AutoTiledTileMap_resize_and_shift_values_remove_cols();
    AutoTiledTileMap_resize_and_shift_values_add_rows_and_cols();
    AutoTiledTileMap_resize_and_shift_values_add_rows_and_remove_cols();
    AutoTiledTileMap_resize_and_shift_values_remove_rows_and_add_cols();
    AutoTiledTileMap_resize_and_shift_values_remove_rows_and_remove_cols();
}