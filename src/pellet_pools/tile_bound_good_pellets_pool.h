#ifndef BIRDMAZEGAME_TILE_BOUND_GOOD_PELLETS_POOL_H
#define BIRDMAZEGAME_TILE_BOUND_GOOD_PELLETS_POOL_H

#include <bitset>
#include <src/util/util_error_handling.h>
#include <src/tile/tilemap.h>


//----------------------------------------------------------
struct TileBoundGoodPelletsPool
{
    Tilemap *tilemap;
    std::bitset<MAX_ROWS_ALLOWED * MAX_COLS_ALLOWED> is_active;

    int first_inactive()
    {
        for(int i = 0; i < tilemap->n_tiles(); i++)
        {
            if(is_active[i] == false)
            {
                return i;
            }
        }
        // Not really sure what to do yet when there are no more inactive pellets, so just going to exit
        // Will figure out later
        ErrorLog("Did not find any inactive pellets!");
        exit(EXIT_FAILURE);
    }

    bool is_active_at_tile(TileIndex ti)
    {
        int i = two_dim_to_one_dim_index(ti.row, ti.col, tilemap->n_cols);
        if(is_active[i])
        {
            return true;
        }
        return false;
    }

    void set_active_at_tile(TileIndex ti)
    {
        int i = two_dim_to_one_dim_index(ti.row, ti.col, tilemap->n_cols);
        is_active[i] = true;
    }

    void set_inactive_at_tile(TileIndex ti)
    {
        int i = two_dim_to_one_dim_index(ti.row, ti.col, tilemap->n_cols);
        is_active[i] = false;
    }
};


//----------------------------------------------------------
TileBoundGoodPelletsPool TileBoundGoodPelletsPoolInit(Tilemap &tilemap);


//----------------------------------------------------------
void TilBoundGoodPelletsPoolCreatePellet(TileBoundGoodPelletsPool &pellets_pool, TileIndex ti);


//----------------------------------------------------------
void TilBoundGoodPelletsPoolRemovePellet(TileBoundGoodPelletsPool &pellets_pool, TileIndex ti);


//----------------------------------------------------------
void TileBoundGoodPelletsPoolRender(TileBoundGoodPelletsPool &pellets_pool, v2d tilemap_offset);

#endif //BIRDMAZEGAME_TILE_BOUND_GOOD_PELLETS_POOL_H
