//
// Created by frog on 8/23/22.
//

#include "tilemap.h"
#include <vector>
#include <src/global.h>
#include <src/util/util_error_handling.h>
#include <src/util/util_draw.h>


bool
TilemapIsCollisionTileAt(Tilemap tilemap, int row, int col)
{
    return std_vector_2d_at<bool>(tilemap.is_collision_tiles, row, col, tilemap.n_cols);
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
bool_vector_2d_resize_and_shift_values(std::vector<bool> &vector, Uint32 curr_n_rows, Uint32 curr_n_cols, Uint32 new_n_rows, Uint32 new_n_cols)
{
    //-----------------------
    //
    // VALIDATE
    //
    //-----------------------

    // too much?

    if(new_n_rows > MAX_ROWS_ALLOWED)
    {
        fprintf(stderr, "Not allowed to set more than %d rows for tilemap\n", MAX_ROWS_ALLOWED);
    }
    if(new_n_cols > MAX_COLS_ALLOWED)
    {
        fprintf(stderr, "Not allowed to set more than %d cols for tilemap\n", MAX_COLS_ALLOWED);
    }

    // not correct values?

    if(curr_n_cols * curr_n_rows != vector.size())
    {
        fprintf(stderr, "Not allowed to pass in unequal vector and curr dimensions\n");
        exit(EXIT_FAILURE);
    }

    // no change?

    if(curr_n_rows == new_n_rows && curr_n_cols == new_n_cols)
    {
        return;
    }


    //-----------------------
    //
    // ACTUAL WORK
    //
    //-----------------------

    int n_rows_difference = new_n_rows - curr_n_rows;
    int n_cols_difference = new_n_cols - curr_n_cols;

    // CHANGE ROW SIZE

    if(n_rows_difference > 0)
    {
        int n_tiles_to_add = n_rows_difference * curr_n_cols;

        for(int i = 0; i < n_tiles_to_add; i++)
        {
            vector.push_back(false);
        }
    }
    else if(n_rows_difference < 0)
    {
        int n_tiles_to_remove = (-1 * n_rows_difference) * curr_n_cols;

        for(int i = 0; i < n_tiles_to_remove; i++)
        {
            vector.pop_back();
        }
    }


    // CHANGE COL SIZE AND SHIFT VALUES FOR COL ALIGNMENT

    if(n_cols_difference > 0)
    {
        int n_tiles_to_add = n_cols_difference * new_n_rows;

        // add tiles
        for(int i = 0; i < n_tiles_to_add; i++)
        {
            vector.push_back(false);
        }

        // shift tiles so that cols correctly align after adding cols
        for(int row = new_n_rows; row >= 0; row--)
        {
            int right_shift_amount = n_cols_difference * row;

            for(int col = curr_n_cols; col >= 0; col--)
            {
                int value = std_vector_2d_at<bool>(vector, row, col, curr_n_cols);
                int index = two_dim_to_one_dim_index(row, col, curr_n_cols);
                int shifted_index = index + right_shift_amount;
                vector[shifted_index] = value;
            }
        }


        // fill in padding
        for(int row = 0; row < new_n_rows; row++)
        {
            for(int col = curr_n_cols; col < new_n_cols; col++)
            {
                std_vector_2d_set<bool>(
                        vector,
                        row,
                        col,
                        new_n_cols,
                        false
                );
            }
        }
    }
    else if(n_cols_difference < 0)
    {
        for(int row = 0; row < new_n_rows; row++)
        {
            int left_shift_amount = row * (-1 *n_cols_difference);

            for(int col = 0; col < curr_n_cols; col++)
            {
                int index = two_dim_to_one_dim_index(row, col, curr_n_cols);
                int value = vector[index];
                int shifted_index = index - left_shift_amount;
                vector[shifted_index] = value;
            }
        }

        int n_tiles_to_remove = (-1 * n_cols_difference) * new_n_rows;

        for(int i = 0; i < n_tiles_to_remove; i++)
        {
            vector.pop_back();
        }
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
            bool is_collidable_tile = (bool) std_vector_2d_at<bool>(tilemap.is_collision_tiles, row, col,
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