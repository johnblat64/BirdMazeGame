//
// Created by frog on 8/23/22.
//

#include "tilemap.h"
#include <vector>


static int
two_dim_to_one_dim_index(int row, int col, int n_cols)
{
    return (row * n_cols) + col;
}

void
set_2d(std::vector<bool> &v, int row, int col, int n_cols, int value)
{
    int index = two_dim_to_one_dim_index(row, col, n_cols);
    v[index] = value;
}

int
at_2d(std::vector<bool> v, int row, int col, int n_cols)
{
    int index = two_dim_to_one_dim_index(row, col, n_cols);
    return v[index];
}


void 
AutoTiledTileMap_set_wall_value(AutoTiledTileMap *tilemap, int row, int col, bool wall_value)
{
    int index = two_dim_to_one_dim_index(row, col, tilemap->n_cols);
    tilemap->walls[index] = wall_value;
}

bool 
AutoTiledTileMap_get_wall_value(AutoTiledTileMap tilemap, int row, int col)
{
    return at_2d(tilemap.walls, row, col, tilemap.n_cols);
}








void
AutoTiledTileMap_resize_and_shift_values(AutoTiledTileMap *tilemap, Uint32 new_n_rows, Uint32 new_n_cols)
{
    // too much?

    if(new_n_rows > MAX_ROWS_ALLOWED)
    {
        fprintf(stderr, "Not allowed to set more than %d rows for tilemap\n", MAX_ROWS_ALLOWED);
    }
    if(new_n_cols > MAX_COLS_ALLOWED)
    {
        fprintf(stderr, "Not allowed to set more than %d cols for tilemap\n", MAX_COLS_ALLOWED);
    }

    // no change?

    if(tilemap->n_rows == new_n_rows && tilemap->n_cols == new_n_cols)
    {
        return;
    }

    // calculate difference between old and new size

    int n_rows_difference = new_n_rows - tilemap->n_rows;
    int n_cols_difference = new_n_cols - tilemap->n_cols;

    // CHANGE ROW SIZE

    if(n_rows_difference > 0)
    {
        int n_tiles_to_add = n_rows_difference * tilemap->n_cols;

        for(int i = 0; i < n_tiles_to_add; i++)
        {
            tilemap->walls.push_back(false);
        }
    }
    else if(n_rows_difference < 0)
    {
        int n_tiles_to_remove = (-1 * n_rows_difference) * tilemap->n_cols;

        for(int i = 0; i < n_tiles_to_remove; i++)
        {
            tilemap->walls.pop_back();
        }
    }

    tilemap->n_rows = new_n_rows;

    // CHANGE COL SIZE AND SHIFT VALUES FOR COL ALIGNMENT

    if(n_cols_difference > 0)
    {
        int n_tiles_to_add = n_cols_difference * tilemap->n_rows;

        // add tiles
        for(int i = 0; i < n_tiles_to_add; i++)
        {
            tilemap->walls.push_back(false);
        }

        // shift tiles so that cols correctly align after adding cols
        for(int row = tilemap->n_rows; row >= 0; row--)
        {
            int right_shift_amount = n_cols_difference * row;

            for(int col = tilemap->n_cols; col >= 0; col--)
            {
                int value = at_2d(tilemap->walls, row, col, tilemap->n_cols);
                int index = two_dim_to_one_dim_index(row, col, tilemap->n_cols);
                int shifted_index = index + right_shift_amount;
                tilemap->walls[shifted_index] = value;
            }
        }

        int old_n_cols = tilemap->n_cols;
        tilemap->n_cols = new_n_cols;

        // fill in padding with no sprite value
        for(int row = 0; row < tilemap->n_rows; row++)
        {
            for(int col = old_n_cols; col < new_n_cols; col++)
            {
                set_2d(
                    tilemap->walls,
                    row,
                    col,
                    tilemap->n_cols,
                    false
                );
            }
        }
    }
    else if(n_cols_difference < 0)
    {
        for(int row = 0; row < tilemap->n_rows; row++)
        {
            int left_shift_amount = row * (-1 *n_cols_difference);

            for(int col = 0; col < tilemap->n_cols; col++)
            {
                int index = two_dim_to_one_dim_index(row, col, tilemap->n_cols);
                int value = tilemap->walls[index];
                int shifted_index = index - left_shift_amount;
                tilemap->walls[shifted_index] = value;
            }
        }

        int n_tiles_to_remove = (-1 * n_cols_difference) * tilemap->n_rows;

        for(int i = 0; i < n_tiles_to_remove; i++)
        {
            tilemap->walls.pop_back();
        }

        tilemap->n_cols = new_n_cols;
    }
}