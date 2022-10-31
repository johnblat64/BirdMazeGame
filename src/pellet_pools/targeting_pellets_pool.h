//
// Created by frog on 10/30/22.
//

#ifndef BIRDMAZEGAME_TARGETING_PELLETS_POOL_H
#define BIRDMAZEGAME_TARGETING_PELLETS_POOL_H

#include <vector>
#include "src/util/util_misc.h"
#include <src/util/util_error_handling.h>


struct TargetingPelletsPool
{
    std::vector<bool> is_active;
    v2d target_tilemap_position;
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
};

#endif //BIRDMAZEGAME_TARGETING_PELLETS_POOL_H
