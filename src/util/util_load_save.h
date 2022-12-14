// util_load_save

#ifndef BIRDMAZEGAME_UTIL_LOAD_SAVE_H
#define BIRDMAZEGAME_UTIL_LOAD_SAVE_H


#include <vector>
#include "SDL2/include/SDL.h"
#include <src/tile/tilemap.h>
#include "src/components/components.h"
#include "util_error_handling.h"
#include <src/pellet_pools/tile_bound_good_pellets_pool.h>
#include <src/tile/tileset.h>


// for now, this is the data we need to start with for the world/first level
// we're basically going to save this to a file using binary writes
// and load it with binary reads
// use the helper functions for each type to assist

enum LoadFileResult
{
    LOAD_FILE_NOT_FOUND,
    LOAD_FULL_BYTES_NOT_READ,
    LOAD_SUCCESS
};

LoadFileResult
TilemapLoadFromFile(const char *filename, Tilemap &tilemap);

bool
TilemapSaveToFile(const char *filename, Tilemap tilemap);

bool
TilesetSaveToFile(const char* filename, Tileset tileset);

bool TilesetLoadTilesetTexture(SDL_Renderer *renderer, Tileset &tileset, std::string file_path);

LoadFileResult
TilesetLoadFromFile(const char *filename, Tileset &tileset);

bool 
TilesetSaveToFile(const char *filename, Tileset tileset);


// -----------------------------------------------------------------
LoadFileResult
TilesetLoadFromFile(const char *filename, Tileset &tileset);

LoadFileResult
TileBoundGoodPelletsPoolLoadFromFile(const char *filename, TileBoundGoodPelletsPool &pellets_pool);

bool
TileBoundGoodPelletsPoolSaveToFile(const char *filename, TileBoundGoodPelletsPool &pellets_pool);

// ---------------------------------------------------------
//
// Saving and loading fucntions will return number of bytes read/written
// Use this value against expected to determine if everything is ok
//
// ---------------------------------------------------------

template <typename T> size_t
util_save_vals_to_context(SDL_RWops *save_context, T *ptr, size_t num_vals)
{
    size_t num_bytes_written = 0;
    num_bytes_written += save_context->write(save_context, (const void *) &num_vals, sizeof(size_t), 1);
    num_bytes_written += save_context->write(save_context, (const void *) ptr, sizeof(T), num_vals);
    return num_bytes_written;
}

template <typename T> size_t
util_save_val_to_context(SDL_RWops *save_context, T *ptr)
{
    size_t num_bytes_written = 0;
    num_bytes_written += save_context->write(save_context, (const void *) ptr, sizeof(T), 1);
    return num_bytes_written;
}


template <typename T> size_t
util_load_val_from_file(SDL_RWops *load_context, T *ptr)
{
    return load_context->read(load_context, (void *) ptr, sizeof(T), 1);
}


template <typename T> size_t
util_load_vals_from_context_to_vector(SDL_RWops *load_context, std::vector<T> &vec)
{
    size_t num_vals;
    size_t total_num_bytes_read = 0;
    size_t num_bytes_read = load_context->read(load_context, (void *) &num_vals, sizeof(size_t), 1);
    if (num_bytes_read <= 0) {
        ErrorLog("no bytes read");
        exit(EXIT_FAILURE);
    }
    total_num_bytes_read += num_bytes_read;

    T *ptr = (T *) malloc(sizeof(T) * num_vals);

    num_bytes_read = load_context->read(load_context, (void *) ptr, sizeof(T), num_vals);

    if (num_bytes_read <= 0) {
        ErrorLog("no bytes read");
        exit(EXIT_FAILURE);
    }
    total_num_bytes_read += num_bytes_read;

    vec.clear();
    for (int i = 0; i < num_vals; i++) {
        vec.push_back(ptr[i]);
    }

    free(ptr);

    return total_num_bytes_read;
}


template <typename T> size_t
util_load_vals_from_context_to_ptr_array(SDL_RWops *load_context, T *ptr, size_t *num_vals)
{

    size_t total_num_bytes_read = 0;
    size_t num_bytes_read = load_context->read(load_context, (void *) num_vals, sizeof(size_t), 1);
    if (num_bytes_read <= 0) {
        ErrorLog("no bytes read");
        exit(EXIT_FAILURE);
    }
    total_num_bytes_read += num_bytes_read;

    ptr = (T *) malloc(sizeof(T) * *num_vals);

    num_bytes_read = load_context->read(load_context, (void *) ptr, sizeof(T), *num_vals);

    if (num_bytes_read <= 0) {
        ErrorLog("no bytes read");
        exit(EXIT_FAILURE);
    }
    total_num_bytes_read += num_bytes_read;


    return total_num_bytes_read;
}

#endif //BIRDMAZEGAME_UTIL_LOAD_SAVE_H
