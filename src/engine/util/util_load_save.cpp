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
    if (write_context == nullptr)
    {
        return false;
    }

    nlohmann::json tilemap_json;
    nlohmann::to_json(tilemap_json, tilemap);
    std::string tilemap_json_str = tilemap_json.dump();
    write_context->write(write_context, tilemap_json_str.c_str(), sizeof(char), tilemap_json_str.size());

    return true;
}

// -----------------------------------------------------------------
bool 
TilesetSaveToFile(const char* filename, Tileset tileset)
{
    SDL_RWops *write_context = SDL_RWFromFile(filename, "wb");
    if (write_context == nullptr)
    {
        return false;
    }

    nlohmann::json tileset_json;
    nlohmann::to_json(tileset_json, tileset);
    std::string tileset_json_str = tileset_json.dump();
    write_context->write(write_context, tileset_json_str.c_str(), sizeof(char), tileset_json_str.size());

    return true;
}


// -----------------------------------------------------------------
LoadFileResult
Tilemap_load_from_file(const char *filename, Tilemap &tilemap)
{
    SDL_RWops *read_context = SDL_RWFromFile(filename, "rb");

    if (read_context == nullptr)
    {
        const char *error = SDL_GetError();
        fprintf(stderr, "%s\n", error);
        return LOAD_FILE_NOT_FOUND;
    }

    size_t file_size = read_context->size(read_context);

    std::string tilemap_json_str;
    tilemap_json_str.resize(file_size);

    read_context->read(read_context, &tilemap_json_str[0], sizeof(char), file_size);

    nlohmann::json tilemap_json = nlohmann::json::parse(tilemap_json_str);

    from_json(tilemap_json, tilemap);

    printf("Loaded TileMap\n");
    SDL_RWclose(read_context);
    return LOAD_SUCCESS;
}

// -----------------------------------------------------------------
LoadFileResult
TilesetLoadFromFile(const char *filename, Tileset &tileset)
{
    SDL_RWops *read_context = SDL_RWFromFile(filename, "rb");

    if (read_context == nullptr)
    {
        const char *error = SDL_GetError();
        fprintf(stderr, "%s\n", error);
        return LOAD_FILE_NOT_FOUND;
    }

    size_t file_size = read_context->size(read_context);

    std::string tileset_json_str;
    tileset_json_str.resize(file_size);

    read_context->read(read_context, &tileset_json_str[0], sizeof(char), file_size);

    nlohmann::json tileset_json = nlohmann::json::parse(tileset_json_str);

    from_json(tileset_json, tileset);

    printf("Loaded Tileset\n");
    SDL_RWclose(read_context);
    return LOAD_SUCCESS;
}

