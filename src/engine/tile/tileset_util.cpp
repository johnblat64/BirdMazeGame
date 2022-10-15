#include "tileset.h"
#include <stdio.h>
#include "stb/stb_image.h"
#include <src/engine/util/util_error_handling.h>

void 
TilesetSetBitMaskTile(Tileset &tileset, int mouse_x, int mouse_y)
{
    Uint8 bitmask_cell_values[3][3] = {
            {0x01, 0x02, 0x04},
            {0x08, 0x00, 0x10},
            {0x20, 0x40, 0x80}
    };
    int row_tile_clicked = mouse_y / tileset.sprite_sheet.cell_height();
    int col_tile_clicked = mouse_x / tileset.sprite_sheet.cell_width();
    
    if (row_tile_clicked < tileset.sprite_sheet.n_rows && col_tile_clicked < tileset.sprite_sheet.n_cols)
    {
        int tile_location_x = col_tile_clicked * tileset.sprite_sheet.cell_width();
        int tile_location_y = row_tile_clicked * tileset.sprite_sheet.cell_height();

        int bit_cell_size_height = tileset.sprite_sheet.cell_height() / 3;
        int bit_cell_size_width = tileset.sprite_sheet.cell_width() / 3;

        int relative_mouse_x = mouse_x - tile_location_x;
        int relative_mouse_y = mouse_y - tile_location_y;

        int bit_clicked_row = relative_mouse_y / bit_cell_size_height;
        int bit_clicked_col = relative_mouse_x / bit_cell_size_width;


        bit_clicked_row = bit_clicked_row > 2 ? 2 : bit_clicked_row;
        bit_clicked_col = bit_clicked_col > 2 ? 2 : bit_clicked_col;
        
        Uint8 clicked_bit_value = bitmask_cell_values[bit_clicked_row][bit_clicked_col];
        Uint8 current_bitmask_value =  tileset.bitmask_get_element(row_tile_clicked, col_tile_clicked);
        current_bitmask_value |= clicked_bit_value;
        tileset.bitmask_set_element(current_bitmask_value, row_tile_clicked, col_tile_clicked);
    }

}

void 
TilesetUnsetBitMaskTile(Tileset &tileset, int mouse_x, int mouse_y)
{
    Uint8 bitmask_cell_values[3][3] = {
            {0x01, 0x02, 0x04},
            {0x08, 0x00, 0x10},
            {0x20, 0x40, 0x80}};

    int row_tile_clicked = mouse_y / tileset.sprite_sheet.cell_height();
    int col_tile_clicked = mouse_x / tileset.sprite_sheet.cell_width();

    if (row_tile_clicked < tileset.sprite_sheet.n_rows && col_tile_clicked < tileset.sprite_sheet.n_cols)
    {
        int tile_location_x = col_tile_clicked * tileset.sprite_sheet.cell_width();
        int tile_location_y = row_tile_clicked * tileset.sprite_sheet.cell_height();

        int bit_cell_size_height = tileset.sprite_sheet.cell_height() / 3;
        int bit_cell_size_width = tileset.sprite_sheet.cell_width() / 3;

        int relative_mouse_x = mouse_x - tile_location_x;
        int relative_mouse_y = mouse_y - tile_location_y;

        int bit_clicked_row = relative_mouse_y / bit_cell_size_height;
        int bit_clicked_col = relative_mouse_x / bit_cell_size_width;

        bit_clicked_row = bit_clicked_row > 2 ? 2 : bit_clicked_row;
        bit_clicked_col = bit_clicked_col > 2 ? 2 : bit_clicked_col;

        Uint8 clicked_bit_value = bitmask_cell_values[bit_clicked_row][bit_clicked_col];
        Uint8 current_bitmask_value = tileset.bitmask_get_element(row_tile_clicked, col_tile_clicked);

        if ((current_bitmask_value & clicked_bit_value) == clicked_bit_value)
        {
            current_bitmask_value ^= clicked_bit_value;
        }
        tileset.bitmask_set_element(current_bitmask_value, row_tile_clicked, col_tile_clicked);
    }
}

bool
TilesetInit(SDL_Renderer *renderer, Tileset& working_tileset, std::string file_path, int rows, int cols)
{
    Tileset tileset;
    int tileset_width, tileset_height, tileset_channels;
    int req_format = STBI_rgb_alpha;
    unsigned char *tileset_image_data = stbi_load(file_path.c_str(), &tileset_width, &tileset_height,
                                                  &tileset_channels, req_format);
    if (tileset_image_data == NULL)
    {
        return false;
    }

    tileset.sprite_sheet.texture_w = (float) tileset_width;
    tileset.sprite_sheet.texture_h = (float) tileset_height;
    tileset.sprite_sheet.n_rows = rows;
    tileset.sprite_sheet.n_cols = cols;
    tileset.bitmasks.reserve(rows * cols);

    int depth, pitch;
    SDL_Surface *image_surface;
    Uint32 pixel_format;

    depth = 32;
    pitch = 4 * tileset_width;
    pixel_format = SDL_PIXELFORMAT_RGBA32;

    image_surface = SDL_CreateRGBSurfaceWithFormatFrom(tileset_image_data, tileset_width, tileset_height, depth,
                                                       pitch, pixel_format);
    SDLErrorHandleNull(image_surface);
    tileset.sprite_sheet.texture = SDL_CreateTextureFromSurface(renderer, image_surface);
    SDLErrorHandleNull(tileset.sprite_sheet.texture);
    SDL_FreeSurface(image_surface);

    stbi_image_free(tileset_image_data);

    for (int i = 0; i < rows * cols; i++) 
    {
        tileset.bitmasks.push_back(0x00);
    }

    working_tileset = tileset;

    return true;

}

void 
BitmaskRender(SDL_Renderer *renderer, Tileset tileset)
{
    SDL_FRect visual_bitmask_rect;

    Uint8 current_tile_bitmask;
    int tile_num = 3;
    float bit_rect_h_size = tileset.sprite_sheet.cell_height() / tile_num;
    float bit_rect_w_size = tileset.sprite_sheet.cell_width() / tile_num;
    visual_bitmask_rect.w = bit_rect_w_size;
    visual_bitmask_rect.h = bit_rect_h_size;
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 120);

    for (int row = 0; row < tileset.sprite_sheet.n_rows; row++)
    {
        for (int col = 0; col < tileset.sprite_sheet.n_cols; col++)
        {
            int bit_rect_x = tileset.sprite_sheet.cell_width() * col;
            int bit_rect_y = tileset.sprite_sheet.cell_height() * row;
            Uint8 value_rightmost_one = 0x01;
            Uint8 value = 0x00;
            unsigned int tileset_index = 0;
            current_tile_bitmask = tileset.bitmask_get_element(row, col);

            for (int i = 0; i < 8; i++)
            {
                value = value_rightmost_one << i;
                tileset_index = i >= 4 ? i + 1 : i;
                if ((value & current_tile_bitmask) == value) 
                {
                    if (tileset_index >= 0 && tileset_index < tile_num)
                    {
                        visual_bitmask_rect.x = bit_rect_x + ((tileset_index % tile_num) * bit_rect_w_size);
                        visual_bitmask_rect.y = bit_rect_y ;
                    }
                    else if (tileset_index >= tile_num && tileset_index < tile_num * 2)
                    {
                        visual_bitmask_rect.x = bit_rect_x + ((tileset_index % tile_num) * bit_rect_w_size);
                        visual_bitmask_rect.y = bit_rect_y + bit_rect_h_size;
                    }
                    else if (tileset_index >= tile_num * 2 && tileset_index < tile_num * 3)
                    {
                        visual_bitmask_rect.x = bit_rect_x + ((tileset_index % tile_num) * bit_rect_w_size);
                        visual_bitmask_rect.y = bit_rect_y + bit_rect_h_size * 2;
                    }
                    SDL_RenderFillRectF(renderer, &visual_bitmask_rect);
                }
            }
            
        }
    }
}

void 
RenderTileset(SDL_Renderer *renderer, Tileset tileset, int x, int y)
{
    SDL_Rect renderQuad = {x, y, tileset.sprite_sheet.texture_w, tileset.sprite_sheet.texture_h};

    SDL_RenderCopy(renderer, tileset.sprite_sheet.texture, NULL, &renderQuad);
}

void 
TilesetResizeandShiftValues(Tileset &tileset, Uint32 new_n_rows, Uint32 new_n_cols)
{
    int row_diff = new_n_rows - tileset.sprite_sheet.n_rows;
    int col_diff = new_n_cols - tileset.sprite_sheet.n_cols;


    if (row_diff == 0 && col_diff == 0) 
    {
        return;
    }

    if (row_diff > 0)
    {
        int tiles_to_add = row_diff * tileset.sprite_sheet.n_cols;
        for (int i = 0; i < tiles_to_add; i++)
        {
            tileset.bitmasks.push_back(0x00);
        }
    }
    else if (row_diff < 0)
    {
        int tiles_to_sub = (-1 * row_diff) * tileset.sprite_sheet.n_cols;
        for (int i = 0; i < tiles_to_sub; i++)
        {
            tileset.bitmasks.pop_back();
        }
    }
    tileset.sprite_sheet.n_rows = new_n_rows;

    if (col_diff > 0)
    {
        int tiles_to_add = col_diff * tileset.sprite_sheet.n_cols;
        for (int i = 0; i < tiles_to_add; i++)
        {
            tileset.bitmasks.push_back(0x00);
        }
        
        
        for (int i = tileset.sprite_sheet.n_rows - 1; i >= 0; i--)
        {
            int shift_amount = col_diff * i;
            for (int j = tileset.sprite_sheet.n_cols - 1; j>= 0; j--)
            {
                Uint8 value = tileset.bitmask_get_element(i, j);
                int index = i * tileset.sprite_sheet.n_cols + j;
                int new_index = index + shift_amount;
                tileset.bitmasks[new_index] = value;
            }
        }

        int old_cols = tileset.sprite_sheet.n_cols;
        tileset.sprite_sheet.n_cols = new_n_cols;

        for (int i = 0; i < tileset.sprite_sheet.n_rows; i++)
        {
            for (int j = old_cols; j < tileset.sprite_sheet.n_cols; j++)
            {
                tileset.bitmask_set_element(0x00, i, j);
            }
        }

    }
    else if (col_diff < 0)
    {
        for (int i = 0; i < tileset.sprite_sheet.n_rows; i++)
        {
            int shift_amount = -1 * col_diff * i;
            for (int j = 0; j < tileset.sprite_sheet.n_cols; j++)
            {
                Uint8 value = tileset.bitmask_get_element(i, j);
                int index = i * tileset.sprite_sheet.n_cols + j;
                int new_index = index - shift_amount;
                tileset.bitmasks[new_index] = value; 
            }
        }

        int tiles_to_sub = (-1 * col_diff) * tileset.sprite_sheet.n_rows;
        for (int i = 0; i < tiles_to_sub; i++)
        {
            tileset.bitmasks.pop_back();
        }

        tileset.sprite_sheet.n_cols = new_n_cols;
    }
}