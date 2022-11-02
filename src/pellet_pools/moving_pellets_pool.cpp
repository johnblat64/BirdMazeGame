//
// Created by frog on 10/31/22.
//

#include <src/pellet_pools/moving_pellets_pool.h>
#include <src/tile/tilemap.h>
#include <src/util/util_misc.h>
#include <src/util/util_draw.h>
#include <src/global.h>


void
MovingPelletsPool::CreatePellet(v2d pos, v2d vel)
{
    int i = first_inactive();
    is_active[i] = true;
    tilemap_positions[i] = pos;
    velocities[i] = vel;
}

MovingPelletsPool
MovingPelletsPoolInit(size_t n_pellets)
{
    MovingPelletsPool pellets_pool;
    pellets_pool.is_active.resize(n_pellets);
    pellets_pool.tilemap_positions.resize(n_pellets);
    pellets_pool.velocities.resize(n_pellets);

    return pellets_pool;
}

void MovingPelletsPoolRender(MovingPelletsPool &pellets_pool, Tilemap &tilemap, v2d tilemap_offset)
{
    for(int i = 0; i < pellets_pool.size(); i++)
    {
        if(pellets_pool.is_active[i] == false)
        {
            continue;
        }


        v2d render_pos;
        render_pos.x = tilemap_offset.x + pellets_pool.tilemap_positions[i].x;
        render_pos.y = tilemap_offset.y + pellets_pool.tilemap_positions[i].y;
        Util::DrawCircleFill(Global::renderer, render_pos.x, render_pos.y, 5, (SDL_Color){217, 194, 237,255});
    }
}

