#include "cmake-build-debug/external/SDL2/include/SDL.h"
#include "util_load_save.h"
#include <stdio.h>
#include <src/pellet_pools/tile_bound_good_pellets_pool.h>
#include <src/tile/tileset.h>
#include "stb/stb_image.h"




// The functions below are probably kinda dumb for many reasons
// 1. https://beej.us/guide/bgc/html/split/file-inputoutput.html#struct-and-number-caveats
// 2. Eventually, I'll need to save all entity data, not just tilemap stuff
//
// This code should only be temporary to start testing out tilemapping features
// The solution is to serialize



// -----------------------------------------------------------------
bool
TilemapSaveToFile(const char *filename, Tilemap tilemap)
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
    SDL_RWclose(write_context);

    return true;
}


// -----------------------------------------------------------------
LoadFileResult
TilemapLoadFromFile(const char *filename, Tilemap &tilemap)
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
bool TilesetLoadTilesetTexture(SDL_Renderer *renderer, Tileset &tileset, std::string file_path)
{
    int tileset_width, tileset_height, tileset_channels;
    int req_format = STBI_rgb_alpha;
    unsigned char *tileset_image_data = stbi_load(file_path.c_str(), &tileset_width, &tileset_height,
                                                  &tileset_channels, req_format);
    tileset.texture_initialized = true;
    if (tileset_image_data == NULL)
    {
        const char *error = SDL_GetError();
        fprintf(stderr, "%s\n", error);
        return LOAD_FILE_NOT_FOUND;
        tileset.sprite_sheet.texture = NULL;
        return false;
    }

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

    return true;
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


// -----------------------------------------------------------------
bool
TileBoundGoodPelletsPoolSaveToFile(const char *filename, TileBoundGoodPelletsPool &pellets_pool)
{
    SDL_RWops *write_context = SDL_RWFromFile(filename, "wb");
    if (write_context == nullptr)
    {
        return false;
    }

    nlohmann::json tilemap_json;
    nlohmann::to_json(tilemap_json, pellets_pool);
    std::string tilemap_json_str = tilemap_json.dump();
    write_context->write(write_context, tilemap_json_str.c_str(), sizeof(char), tilemap_json_str.size());

    return true;
}

// -----------------------------------------------------------------
LoadFileResult
TileBoundGoodPelletsPoolLoadFromFile(const char *filename, TileBoundGoodPelletsPool &pellets_pool)
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

    from_json(tilemap_json, pellets_pool);

    printf("Loaded Pellets Pool\n");
    SDL_RWclose(read_context);
    return LOAD_SUCCESS;
}