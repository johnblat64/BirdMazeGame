#include <stddef.h>
#include <bitset>
#include <src/util/util_error_handling.h>
#include <src/tile/tilemap.h>
#include <src/util/util_draw.h>
#include <src/global.h>
#include <src/pellet_pools/tile_bound_good_pellets_pool.h>



//----------------------------------------------------------
TileBoundGoodPelletsPool TileBoundGoodPelletsPoolInit(Tilemap &tilemap)
{
    TileBoundGoodPelletsPool pellets_pool;
    pellets_pool.tilemap = &tilemap;
    pellets_pool.is_active.reset();

    return pellets_pool;
}


//----------------------------------------------------------
void TilBoundGoodPelletsPoolCreatePellet(TileBoundGoodPelletsPool &pellets_pool, TileIndex ti)
{
    int i = ti.to_one_dim_index(pellets_pool.tilemap->n_cols);
    pellets_pool.is_active[i] = true;
}


//----------------------------------------------------------
void TilBoundGoodPelletsPoolRemovePellet(TileBoundGoodPelletsPool &pellets_pool, TileIndex ti)
{
    int i = ti.to_one_dim_index(pellets_pool.tilemap->n_cols);
    pellets_pool.is_active[i] = false;
}



//----------------------------------------------------------
void TileBoundGoodPelletsPoolRender(TileBoundGoodPelletsPool &pellets_pool, v2d tilemap_offset)
{
    float wave_height = 5;
    float wave_speed = 10;

    for(int i = 0; i < pellets_pool.tilemap->n_tiles(); i++)
    {
        if(pellets_pool.is_active[i] == false)
        {
            continue;
        }

        TileIndex ti = {i / (int)pellets_pool.tilemap->n_cols,
                        i % (int)pellets_pool.tilemap->n_cols};
        v2d tile_center_pos = pellets_pool.tilemap->center_pos_of_tile(ti.row, ti.col);
        v2d render_pos;
        render_pos.x = tilemap_offset.x + tile_center_pos.x;
        render_pos.y = tilemap_offset.y + tile_center_pos.y + sin((float)SDL_GetTicks() / 100.0f + ((float)i / 10.0f) * wave_speed) * wave_height;
        Util::DrawCircleFill(Global::renderer, render_pos.x, render_pos.y, 5, (SDL_Color){255,0,255,255});
    }
}