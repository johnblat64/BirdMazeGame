//
// Created by frog on 8/23/22.
//

#include "tilemap.h"
#include <vector>
#include <src/engine/global.h>
#include <src/engine/util/util_error_handling.h>
#include <src/engine/util/util_draw.h>


bool
TilemapIsCollisionTileAt(Tilemap tilemap, int row, int col)
{
    return std_vector_2d_at<char>(tilemap.is_collision_tiles, row, col, tilemap.n_cols);
}


int
two_dim_to_one_dim_index(int row, int col, int n_cols)
{
    return (row * n_cols) + col;
}


void
Tilemap_set_collision_tile_value(Tilemap *tilemap, int row, int col, bool is_collision_tile)
{
    int index = two_dim_to_one_dim_index(row, col, tilemap->n_cols);
    tilemap->is_collision_tiles[index] = is_collision_tile;
}



void
Tilemap_resize_and_shift_values(Tilemap *tilemap, Uint32 new_n_rows, Uint32 new_n_cols)
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
            tilemap->is_collision_tiles.push_back(false);
        }
    }
    else if(n_rows_difference < 0)
    {
        int n_tiles_to_remove = (-1 * n_rows_difference) * tilemap->n_cols;

        for(int i = 0; i < n_tiles_to_remove; i++)
        {
            tilemap->is_collision_tiles.pop_back();
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
            tilemap->is_collision_tiles.push_back(false);
        }

        // shift tiles so that cols correctly align after adding cols
        for(int row = tilemap->n_rows; row >= 0; row--)
        {
            int right_shift_amount = n_cols_difference * row;

            for(int col = tilemap->n_cols; col >= 0; col--)
            {
                int value = std_vector_2d_at(tilemap->is_collision_tiles, row, col, tilemap->n_cols);
                int index = two_dim_to_one_dim_index(row, col, tilemap->n_cols);
                int shifted_index = index + right_shift_amount;
                tilemap->is_collision_tiles[shifted_index] = value;
            }
        }

        int old_n_cols = tilemap->n_cols;
        tilemap->n_cols = new_n_cols;

        // fill in padding with no sprite value
        for(int row = 0; row < tilemap->n_rows; row++)
        {
            for(int col = old_n_cols; col < new_n_cols; col++)
            {
                std_vector_2d_set(
                        tilemap->is_collision_tiles,
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
                int value = tilemap->is_collision_tiles[index];
                int shifted_index = index - left_shift_amount;
                tilemap->is_collision_tiles[shifted_index] = value;
            }
        }

        int n_tiles_to_remove = (-1 * n_cols_difference) * tilemap->n_rows;

        for(int i = 0; i < n_tiles_to_remove; i++)
        {
            tilemap->is_collision_tiles.pop_back();
        }

        tilemap->n_cols = new_n_cols;
    }
}


void
TilemapCollisionTileRectsRender(Tilemap &tilemap, float pos_x, float pos_y, SDL_Color color)
{
    SDLErrorHandle(SDL_SetRenderDrawColor(Global::renderer, color.r, color.g, color.b, color.a));

    for (int row = 0; row < tilemap.n_rows; row++)
    {
        for (int col = 0; col < tilemap.n_cols; col++)
        {
            bool is_collidable_tile = (bool) std_vector_2d_at<char>(tilemap.is_collision_tiles, row, col,
                                                                    tilemap.n_cols);
            if (is_collidable_tile)
            {
                SDL_FRect collidable_tile_rect = {
                        pos_x + (float) (col * tilemap.tile_size),
                        pos_y + (float) (row * tilemap.tile_size),
                        (float) tilemap.tile_size,
                        (float) tilemap.tile_size};

                SDLErrorHandle(SDL_RenderFillRectF(Global::renderer, &collidable_tile_rect));
            }
        }
    }
}


void
TilemapGridRender(Tilemap &tilemap, float pos_x, float pos_y, SDL_Color color)
{
    Util::DrawGrid(Global::renderer,
                   tilemap.tile_size,
                   pos_x,
                   pos_y,
                   tilemap.n_rows,
                   tilemap.n_cols,
                   color);

    // this grid is for showing the center axes for the tilemap which is what the player is snapped to
    color.r *= 0.7f;
    color.g *= 0.7f;
    color.b *= 0.7f;

    Util::DrawGrid(Global::renderer, tilemap.tile_size, pos_x + tilemap.tile_size/2, pos_y + tilemap.tile_size/2, tilemap.n_rows - 1, tilemap.n_cols - 1, color);
}