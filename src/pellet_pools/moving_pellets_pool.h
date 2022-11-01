//
// Created by frog on 10/30/22.
//

#ifndef BIRDMAZEGAME_MOVING_PELLETS_POOL_H
#define BIRDMAZEGAME_MOVING_PELLETS_POOL_H

#include <vector>
#include "src/util/util_misc.h"
#include "src/tile/tilemap.h"
#include <src/util/util_error_handling.h>


struct MovingPelletsPool
{
    std::vector<bool> is_active;
    std::vector<v2d> tilemap_positions;
    std::vector<v2d> velocities;

    int first_inactive()
    {
        for(int i = 0; i < is_active.size(); i++)
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

    size_t size()
    {
        // validate
        size_t x = is_active.size();
        size_t y = tilemap_positions.size();
        size_t z = velocities.size();
        if(x != y && x != z)
        {
            printf("Sizes of vectors are not equal in moving pellets pool something went wrong!\n");
            exit(EXIT_FAILURE);
        }

        return is_active.size();
    }

    void move_all(double delta_time)
    {
        for(int i = 0; i < size(); i++)
        {
            if(is_active[i] == true)
            {
                tilemap_positions[i].x += velocities[i].x * delta_time;
                tilemap_positions[i].y += velocities[i].y * delta_time;
            }
        }
    }


    void
    CreatePellet(v2d pos, v2d vel);
};

//----------------------------------------------------
MovingPelletsPool
MovingPelletsPoolInit(size_t n_pellets);

//-----------------------------------------------------
void
MovingPelletsPoolRender(MovingPelletsPool &pellets_pool, Tilemap &tilemap, v2d tilemap_offset);



#endif //BIRDMAZEGAME_MOVING_PELLETS_POOL_H
