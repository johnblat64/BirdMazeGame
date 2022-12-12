#include <src/editor/level_data.h>
#include <src/util/util_load_save.h>
#include <external/SDL2/include/SDL.h>
#include <src/global.h>


void LevelDataLoadFromFiles(LevelData& level_data)
{

    LoadFileResult load_tilemap_result = TilemapLoadFromFile("assets/tilemap.json", level_data.tilemap);

    if (load_tilemap_result == LOAD_FILE_NOT_FOUND)
    {
        // just initialize it to something
        level_data.tilemap = Tilemap_init(30, 25, 25);
    }
    else if (load_tilemap_result == LOAD_FULL_BYTES_NOT_READ)
    {
        exit(EXIT_FAILURE);
    }

    LoadFileResult load_tileset_result = TilesetLoadFromFile("assets/tileset.json", level_data.tileset);

    if (load_tileset_result == LOAD_FILE_NOT_FOUND)
    {
        level_data.tileset = TilesetInit(8, 20);
    }
    else if (load_tileset_result == LOAD_FULL_BYTES_NOT_READ)
    {
        exit(EXIT_FAILURE);
    }

    bool success = TilesetLoadTilesetTexture(Global::renderer, level_data.tileset, "assets" + level_data.tileset.file_name);

  

    LoadFileResult pellets_pool_load_result = TileBoundGoodPelletsPoolLoadFromFile("assets/tileboundgoodpelletspool.json", level_data.pellets_pool);

    if (pellets_pool_load_result == LOAD_FILE_NOT_FOUND)
    {
        // just initialize it to something
        level_data.pellets_pool = TileBoundGoodPelletsPoolInit(level_data.tilemap.n_tiles());
    }
    else if (pellets_pool_load_result == LOAD_FULL_BYTES_NOT_READ)
    {
        exit(EXIT_FAILURE);
    }
}