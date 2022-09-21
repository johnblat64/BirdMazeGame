#include <vector>
#include <SDL.h>
#include <cassert>
#include "src/engine/util/util_load_save.h"
#include <stdio.h>


// The functions below are probably kinda dumb for many reasons
// 1. https://beej.us/guide/bgc/html/split/file-inputoutput.html#struct-and-number-caveats
// 2. Eventually, I'll need to save all entity data, not just tilemap stuff
//
// This code should only be temporary to start testing out tilemapping features
// The solution is to serialize



// -----------------------------------------------------------------
bool
Tilemap_save_to_file(const char *filename, Tilemap tilemap)
{
    SDL_RWops *write_context = SDL_RWFromFile(filename, "wb");
    if(write_context == NULL)
    {
        return false;
    }
    util_save_vals_to_context<int>(
            write_context,
            tilemap.tileset_sprite_sheet_indices.data(),
            tilemap.tileset_sprite_sheet_indices.size());
    util_save_vals_to_context<char>(write_context, tilemap.is_collision_tiles.data(), tilemap.is_collision_tiles.size());
    util_save_val_to_context<Uint32>(write_context, &tilemap.tile_size);
    util_save_val_to_context<Uint32>(write_context, &tilemap.n_rows);
    util_save_val_to_context<Uint32>(write_context, &tilemap.n_cols);

    return true;
}


// -----------------------------------------------------------------
LoadFileResult
Tilemap_load_from_file(const char *filename, Tilemap &tilemap)
{
    SDL_RWops *read_context = SDL_RWFromFile(filename, "rb");

    if(read_context == nullptr)
    {
        const char *error = SDL_GetError();
        fprintf(stderr, "%s\n",  error);
        return LOAD_FILE_NOT_FOUND;
    }

    util_load_vals_from_context_to_vector<int>(read_context,tilemap.tileset_sprite_sheet_indices);
    util_load_vals_from_context_to_vector<char>(read_context, tilemap.is_collision_tiles);
    util_load_val_from_file<Uint32>(read_context, &tilemap.tile_size);
    util_load_val_from_file<Uint32>(read_context, &tilemap.n_rows);
    util_load_val_from_file<Uint32>(read_context, &tilemap.n_cols);

    printf("Loaded TileMap");
    return LOAD_SUCCESS;
}