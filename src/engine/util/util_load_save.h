#ifndef BIRDMAZEGAME_UTIL_LOAD_SAVE_H
#define BIRDMAZEGAME_UTIL_LOAD_SAVE_H


#include <vector>
#include <SDL.h>


enum LoadFileResult
{
    LOAD_FILE_NOT_FOUND,
    LOAD_FULL_BYTES_NOT_READ,
    LOAD_SUCCESS
};


bool
util_save_AutoTiledTileMap_walls(std::vector<bool> walls, Uint16 n_rows, Uint16 n_cols);

LoadFileResult
util_load_AutoTiledTileMap_walls(std::vector<bool> &walls, Uint16 &n_rows, Uint16 &n_cols);


#endif //BIRDMAZEGAME_UTIL_LOAD_SAVE_H
