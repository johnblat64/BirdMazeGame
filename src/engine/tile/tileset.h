#pragma once
#include <external/SDL2/include/SDL.h>
#include <stdio.h>

struct Tileset 
{
    Uint32 tile_size;
    Uint32 n_rows;
    Uint32 n_cols;
    Uint8 *m_Bitmask;

    Uint8
    Bitmask_Get_Element(int row, int col)
    {
        return m_Bitmask[row * n_cols + col];
    }

    void 
    Bitmask_Set_Element(Uint8 value, int row, int col)
    {
        m_Bitmask[row * n_cols + col] = value;
    }

};



void Tileset_Set_BitMask_Tile(Tileset &tileset, int mouseX, int mouseY);

void Tileset_Unset_BitMask_Tile(Tileset &tileset, int mouseX, int mouseY);

Tileset Tileset_Init(Uint32 tile_size, Uint32 n_rows, Uint32 n_cols);

void Tileset_Free(Tileset &tileset);

void Bitmask_Render(SDL_Renderer *renderer, Tileset tileset);
