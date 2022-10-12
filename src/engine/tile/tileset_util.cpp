#include "tileset.h"
#include <stdio.h>

void 
Tileset_Set_BitMask_Tile(Tileset &tileset, int mouseX, int mouseY)
{
    if (tileset.m_Bitmask == NULL)
        return;
    Uint8 bitmask_cell_values[3][3] = {
            {0x01, 0x02, 0x04},
            {0x08, 0x00, 0x10},
            {0x20, 0x40, 0x80}
    };
    int row_tile_clicked = mouseY / tileset.tile_size;
    int col_tile_clicked = mouseX / tileset.tile_size;

    if (row_tile_clicked < tileset.n_rows && col_tile_clicked < tileset.n_cols)
    {
        int tile_location_x = col_tile_clicked * tileset.tile_size;
        int tile_location_y = row_tile_clicked * tileset.tile_size;

        int bit_cell_size = tileset.tile_size / 3;
        int relative_mouse_x = mouseX - tile_location_x;
        int relative_mouse_y = mouseY - tile_location_y;

        int bit_clicked_row = relative_mouse_y / bit_cell_size;
        int bit_clicked_col = relative_mouse_x / bit_cell_size;
        
        Uint8 clicked_bit_value = bitmask_cell_values[bit_clicked_row][bit_clicked_col];
        Uint8 current_bitmask_value =  tileset.Bitmask_Get_Element(row_tile_clicked, col_tile_clicked);
        current_bitmask_value |= clicked_bit_value;
        tileset.Bitmask_Set_Element(current_bitmask_value, row_tile_clicked, col_tile_clicked);
    }

}

void 
Tileset_Unset_BitMask_Tile(Tileset &tileset, int mouseX, int mouseY)
{
    if (tileset.m_Bitmask == NULL)
        return;
    Uint8 bitmask_cell_values[3][3] = {
            {0x01, 0x02, 0x04},
            {0x08, 0x00, 0x10},
            {0x20, 0x40, 0x80}};

    int row_tile_clicked = mouseY / tileset.tile_size;
    int col_tile_clicked = mouseX / tileset.tile_size;

    if (row_tile_clicked < tileset.n_rows && col_tile_clicked < tileset.n_cols)
    {
        int tile_location_x = col_tile_clicked * tileset.tile_size;
        int tile_location_y = row_tile_clicked * tileset.tile_size;

        int bit_cell_size = tileset.tile_size / 3;
        int relative_mouse_x = mouseX - tile_location_x;
        int relative_mouse_y = mouseY - tile_location_y;

        int bit_clicked_row = relative_mouse_y / bit_cell_size;
        int bit_clicked_col = relative_mouse_x / bit_cell_size;

        Uint8 clicked_bit_value = bitmask_cell_values[bit_clicked_row][bit_clicked_col];
        Uint8 current_bitmask_value = tileset.Bitmask_Get_Element(row_tile_clicked, col_tile_clicked);

        if ((current_bitmask_value & clicked_bit_value) == clicked_bit_value)
        {
            current_bitmask_value ^= clicked_bit_value;
        }
        tileset.Bitmask_Set_Element(current_bitmask_value, row_tile_clicked, col_tile_clicked);
    }
}

Tileset
Tileset_Init(Uint32 tile_size, Uint32 n_rows, Uint32 n_cols)
{
    Tileset tileset;
    tileset.n_rows = n_rows;
    tileset.n_cols = n_cols;
    tileset.tile_size = tile_size;

    tileset.m_Bitmask = (Uint8 *) malloc(sizeof(Uint8) * n_rows * n_cols);
    for (int row = 0; row < n_rows; row++)
    {
        for (int col = 0; col < n_cols; col++)
        {
            tileset.Bitmask_Set_Element(0x00, row, col);
        }
    }
    return tileset;

}

void 
Tileset_Free(Tileset &tileset) 
{
    if (tileset.m_Bitmask == NULL)
        return;
    else
        free(tileset.m_Bitmask);
}

void 
Bitmask_Render(SDL_Renderer *renderer, Tileset tileset)
{
    SDL_Rect visual_bitmask_rect;

    Uint8 current_tile_bitmask;
    int tile_num = 3;
    int bit_rect_size = tileset.tile_size / tile_num;
    visual_bitmask_rect.w = bit_rect_size;
    visual_bitmask_rect.h = bit_rect_size;
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 120);

    for (int row = 0; row < tileset.n_rows; row++)
    {
        for (int col = 0; col < tileset.n_cols; col++)
        {
            int bit_rect_x = tileset.tile_size * col;
            int bit_rect_y = tileset.tile_size * row;
            Uint8 value_rightmost_one = 0x01;
            Uint8 value = 0x00;
            unsigned int tileset_index = 0;
            current_tile_bitmask = tileset.Bitmask_Get_Element(row, col);

            for (int i = 0; i < 8; i++)
            {
                value = value_rightmost_one << i;
                tileset_index = i >= 4 ? i + 1 : i;
                if ((value & current_tile_bitmask) == value) 
                {
                    if (tileset_index >= 0 && tileset_index < tile_num)
                    {
                        visual_bitmask_rect.x = bit_rect_x + ((tileset_index % tile_num) * bit_rect_size);
                        visual_bitmask_rect.y = bit_rect_y ;
                    }
                    else if (tileset_index >= tile_num && tileset_index < tile_num * 2)
                    {
                        visual_bitmask_rect.x = bit_rect_x + ((tileset_index % tile_num) * bit_rect_size);
                        visual_bitmask_rect.y = bit_rect_y + bit_rect_size;
                    }
                    else if (tileset_index >= tile_num * 2 && tileset_index < tile_num * 3)
                    {
                        visual_bitmask_rect.x = bit_rect_x + ((tileset_index % tile_num) * bit_rect_size);
                        visual_bitmask_rect.y = bit_rect_y + bit_rect_size * 2;
                    }
                    SDL_RenderFillRect(renderer, &visual_bitmask_rect);
                }
            }
            
        }
    }
}

