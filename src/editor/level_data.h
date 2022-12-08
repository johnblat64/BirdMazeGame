#pragma once

#include <src/tile/tilemap.h>
#include <src/tile/tileset.h>
#include <src/pellet_pools/tile_bound_good_pellets_pool.h>


struct LevelData {
    Tilemap tilemap;
    Tileset tileset;
    TileBoundGoodPelletsPool pellets_pool;
};

void LevelDataLoadFromFiles(LevelData &level_data);
