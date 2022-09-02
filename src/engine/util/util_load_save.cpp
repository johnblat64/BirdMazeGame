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


//---------------------------------------------------------------------------------------------------------------------
// returns true if save was successful. Returns false if not.
bool
util_save_AutoTiledTileMap_walls(std::vector<bool> walls, Uint16 n_rows, Uint16 n_cols)
{
    assert(walls.size() == n_rows * n_cols); // make sure length of walls is correct

    std::vector<char> walls_as_char;
    for(int i = 0; i < walls.size(); i++)
    {
        walls_as_char.push_back(walls[i]);
    } // why? https://stackoverflow.com/questions/46115669/why-does-stdvectorbool-have-no-data

    /**
     * Format of file:
     * n_rows n_cols vector data
     */
    SDL_RWops *write_context = SDL_RWFromFile("assets/auto-tiled-tile-map-walls-binary-data", "wb");

    if(write_context == nullptr)
    {
        const char *error = SDL_GetError();
        fprintf(stderr, "%s\n",  error);
        exit(EXIT_FAILURE);
    }

    size_t num_bytes_written = 0;
    num_bytes_written += write_context->write(write_context, (const void *) &n_rows, sizeof(Uint16), 1);
    num_bytes_written += write_context->write(write_context, (const void *) &n_cols, sizeof(Uint16), 1);
    num_bytes_written += write_context->write(write_context, (const void *) walls_as_char.data(), sizeof(char), n_rows * n_cols);

    write_context->close(write_context);

    size_t expected_num_bytes_written = sizeof(n_rows) + sizeof(n_cols) + ( sizeof(char) * (n_rows * n_cols) );

    if(num_bytes_written == expected_num_bytes_written)
    {
        printf("Saved TileMap");
        return true;
    }

    return false;
}




//---------------------------------------------------------------------------------------------------------------------
LoadFileResult
util_load_AutoTiledTileMap_walls(std::vector<bool> &walls, Uint16 &n_rows, Uint16 &n_cols)
{
    walls.clear();
    char *wall_data_as_chars = (char *) malloc(sizeof(char) * (n_rows * n_cols)); // why? https://stackoverflow.com/questions/46115669/why-does-stdvectorbool-have-no-data

    /**
     * Format of file:
     * n_rows n_cols vector data
     */
    SDL_RWops *read_context = SDL_RWFromFile("assets/auto-tiled-tile-map-walls-binary-data", "rb");

    if(read_context == nullptr)
    {
        const char *error = SDL_GetError();
        fprintf(stderr, "%s\n",  error);
        return LOAD_FILE_NOT_FOUND;
    }

    size_t num_bytes_read = 0;
    num_bytes_read += read_context->read(read_context, (void *) &n_rows, sizeof(Uint16), 1);
    num_bytes_read += read_context->read(read_context, (void *) &n_cols, sizeof(Uint16), 1);
    num_bytes_read += read_context->read(read_context, wall_data_as_chars, sizeof(char), n_rows * n_cols);

    read_context->close(read_context);

    size_t expected_num_bytes_read = sizeof(n_rows) + sizeof(n_cols) + (sizeof(char) * (n_rows * n_cols));
    if(num_bytes_read == expected_num_bytes_read)
    {
        free(wall_data_as_chars);
        return LOAD_FULL_BYTES_NOT_READ;
    }

    for(int i = 0; i < n_rows * n_cols; i++)
    {
        walls.push_back((bool)wall_data_as_chars[i]);
    }

    free(wall_data_as_chars);
    printf("Loaded TileMap");
    return LOAD_SUCCESS;
}