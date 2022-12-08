#include <bitset>
#include <src/pellet_pools/tile_bound_good_pellets_pool.h>
#include <src/util/util_error_handling.h>
#include "src/global.h"
#include <src/sprite/sprite_sheet.h>
#include "src/tile/tilemap.h"
#include "src/util/util_load_save.h"
#include <src/util/util_draw.h>
#include <math.h>
#include <src/util/util_misc.h>
#include <src/pellet_pools/moving_pellets_pool.h>


#define STB_IMAGE_IMPLEMENTATION

#include <external/stb/stb_image.h>


//-----------------------------------------------------------------------------------------------
int sign(float x)
{
    return x >= 0.0 ? 1 : -1;
}


double degrees_to_rads(double degrees)
{
    return degrees * 3.14f / 180.0f;
}


int mod(int a, int b) // C++ messes us negative modding with '%' operator, so use our own
{
    return (b + (a % b)) % b;
}


//-----------------------------------------------------------------------------------------------
void
GameSDLSetup()
{
    SDLErrorHandle(SDL_Init(SDL_INIT_VIDEO));
    Global::window = SDL_CreateWindow(
            "Bird Maze Game",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            Global::window_w,
            Global::window_h,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    SDLErrorHandleNull(Global::window);
    Global::renderer = SDL_CreateRenderer(
            Global::window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    SDLErrorHandleNull(Global::renderer);

}


//-----------------------------------------------------------------------------------------------
enum Axis
{
    AXIS_X,
    AXIS_Y
};


struct Player
{
    AnimatedSprite animated_sprite;
    v2d sprite_pos;
    v2d local_tilemap_pos;
    v2d vel;
    float speed;
    Axis snap_axis; // there is an imaginary x and y-axis running along the tilemap at the centers of each tile. This variable determines what axis the player wants to snap to.


    TileIndex
    current_tile(Tilemap tilemap)
    {
        int wrapped_tilemap_pos_x = mod(local_tilemap_pos.x, tilemap.width());
        int wrapped_tilemap_pos_y = mod(local_tilemap_pos.y, tilemap.height());

        TileIndex ti;
        ti.col = (int) wrapped_tilemap_pos_x / tilemap.tile_size;
        ti.row = (int) wrapped_tilemap_pos_y / tilemap.tile_size;

        return ti;
    }

    TileIndex
    tile_above(Tilemap tilemap)
    {
        TileIndex ti = current_tile(tilemap);
        ti.row--;
        ti.row = mod(ti.row, tilemap.n_rows);

        return ti;

    }

    TileIndex
    tile_below(Tilemap tilemap)
    {
        TileIndex ti = current_tile(tilemap);
        ti.row++;
        ti.row = mod(ti.row, tilemap.n_rows);

        return ti;

    }

    TileIndex
    tile_to_left(Tilemap tilemap)
    {
        TileIndex ti = current_tile(tilemap);
        ti.col--;

        ti.col = mod(ti.col, tilemap.n_cols);

        return ti;

    }

    TileIndex
    tile_to_right(Tilemap tilemap)
    {
        TileIndex ti = current_tile(tilemap);
        ti.col++;

        ti.col = mod(ti.col, tilemap.n_cols);

        return ti;

    }
};


//-----------------------------------------------
bool VelocityIsUp(v2d vel)
{
    return vel.y < 0.0f;
}

bool VelocityIsDown(v2d vel)
{
    return vel.y > 0.0f;
}

bool VelocityIsLeft(v2d vel)
{
    return vel.x < 0.0f;
}

bool VelocityIsRight(v2d vel)
{
    return vel.x > 0.0f;
}


//-----------------------------------------------------------------------------------------------
// local_sprite_pos  of {0, 0) would be directly positioned over the players own position.
Player
PlayerInit(AnimatedSprite animated_sprite, float local_sprite_pos_x, float local_sprite_pos_y, int start_tile_x,
           int start_tile_y, float speed, Tilemap tilemap_inside_of)
{
    Player player;
    player.animated_sprite = animated_sprite;
    player.sprite_pos.x = local_sprite_pos_x;
    player.sprite_pos.y = local_sprite_pos_y;
    player.local_tilemap_pos = tilemap_inside_of.center_pos_of_tile(start_tile_y, start_tile_x);
    player.vel.x = 0.0f;
    player.vel.y = 0.0f;
    player.speed = speed;
    player.snap_axis = AXIS_X;

    return player;
}


//-----------------------------------------------------------------------------------------------
void
PlayerSetVelocityAndSetSnapAxisBasedOnInputAndSpeed(Player &player, Tilemap tilemap, const Uint8 *keyboard_state)
{
    TileIndex tile_above_index = player.tile_above(tilemap);
    TileIndex tile_below_index = player.tile_below(tilemap);
    TileIndex tile_to_left_index = player.tile_to_left(tilemap);
    TileIndex tile_to_right_index = player.tile_to_right(tilemap);

    if (keyboard_state[SDL_SCANCODE_UP]
        && !TilemapIsCollisionTileAt(tilemap, tile_above_index.row, tile_above_index.col))
    {
        player.vel.y = -player.speed;
        player.vel.x = 0.0f;
        player.snap_axis = AXIS_Y;
    }
    else if (keyboard_state[SDL_SCANCODE_DOWN]
             && !TilemapIsCollisionTileAt(tilemap, tile_below_index.row, tile_below_index.col))
    {
        player.vel.y = player.speed;
        player.vel.x = 0.0f;
        player.snap_axis = AXIS_Y;

    }
    else if (keyboard_state[SDL_SCANCODE_LEFT]
             && !TilemapIsCollisionTileAt(tilemap, tile_to_left_index.row, tile_to_left_index.col))
    {
        player.vel.x = -player.speed;
        player.vel.y = 0.0f;
        player.animated_sprite.flip_horizonatally = false;
        player.snap_axis = AXIS_X;
    }
    else if (keyboard_state[SDL_SCANCODE_RIGHT]
             && !TilemapIsCollisionTileAt(tilemap, tile_to_right_index.row, tile_to_right_index.col))
    {
        player.vel.x = player.speed;
        player.vel.y = 0.0f;
        player.animated_sprite.flip_horizonatally = true;
        player.snap_axis = AXIS_X;
    }
}


//-----------------------------------------------------------------------------------------------
void
PlayerSetVelocityToSnapToAxis(Player &player, Tilemap tilemap)
{
    TileIndex current_tile = player.current_tile(tilemap);
    v2d curr_tile_center_pos = tilemap.center_pos_of_tile(current_tile.row, current_tile.col);

    if (player.local_tilemap_pos.x > curr_tile_center_pos.x
        && player.snap_axis == AXIS_Y)
    {
        player.vel.y = degrees_to_rads(45.0f) * player.speed * sign(player.vel.y);
        player.vel.x = degrees_to_rads(45.0f) * -player.speed;
    }
    else if (player.local_tilemap_pos.x < curr_tile_center_pos.x
             && player.snap_axis == AXIS_Y)
    {
        player.vel.y = degrees_to_rads(45.0f) * player.speed * sign(player.vel.y);
        player.vel.x = degrees_to_rads(45.0f) * player.speed;
    }

    if (player.local_tilemap_pos.y > curr_tile_center_pos.y
        && player.snap_axis == AXIS_X)
    {
        player.vel.y = degrees_to_rads(45.0f) * -player.speed;
        player.vel.x = degrees_to_rads(45.0f) * player.speed * sign(player.vel.x);
    }
    else if (player.local_tilemap_pos.y < curr_tile_center_pos.y
             && player.snap_axis == AXIS_X)
    {
        player.vel.y = degrees_to_rads(45.0f) * player.speed;
        player.vel.x = degrees_to_rads(45.0f) * player.speed * sign(player.vel.x);
    }
}


//-----------------------------------------------------------------------------------------------
void
PlayerMove(Player &player)
{
    player.local_tilemap_pos.x += player.vel.x;
    player.local_tilemap_pos.y += player.vel.y;
}


//-----------------------------------------------------------------------------------------------
void
PlayerSetPositionAndSetVelocityOnceFullySnappedOnAxis(Player &player, Tilemap tilemap)
{
    TileIndex curr_tile = player.current_tile(tilemap);
    v2d curr_tile_center_pos = tilemap.center_pos_of_tile(curr_tile.row, curr_tile.col);

    if (player.snap_axis == AXIS_Y
        && VelocityIsRight(player.vel)
        && player.local_tilemap_pos.x > curr_tile_center_pos.x)
    {
        player.local_tilemap_pos.x = curr_tile_center_pos.x;
        player.vel.x = 0.0f;
        player.vel.y = sign(player.vel.y) * player.speed;
    }
    else if (player.snap_axis == AXIS_Y
             && VelocityIsLeft(player.vel)
             && player.local_tilemap_pos.x < curr_tile_center_pos.x)
    {
        player.local_tilemap_pos.x = curr_tile_center_pos.x;
        player.vel.x = 0.0f;
        player.vel.y = sign(player.vel.y) * player.speed;
    }
    else if (player.snap_axis == AXIS_X
             && VelocityIsUp(player.vel)
             && player.local_tilemap_pos.y < curr_tile_center_pos.y)
    {
        player.local_tilemap_pos.y = curr_tile_center_pos.y;
        player.vel.y = 0.0f;
        player.vel.x = sign(player.vel.x) * player.speed;
    }
    else if (player.snap_axis == AXIS_X
             && VelocityIsDown(player.vel)
             && player.local_tilemap_pos.y > curr_tile_center_pos.y)
    {
        player.local_tilemap_pos.y = curr_tile_center_pos.y;
        player.vel.y = 0.0f;
        player.vel.x = sign(player.vel.x) * player.speed;
    }

}


//-----------------------------------------------------------------------------------------------
void
PlayerTilemapCollisionHandle(Player &player, Tilemap tilemap)
{
    TileIndex tile_curr_index = player.current_tile(tilemap);
    TileIndex tile_above_index = player.tile_above(tilemap);
    TileIndex tile_below_index = player.tile_below(tilemap);
    TileIndex tile_to_left_index = player.tile_to_left(tilemap);
    TileIndex tile_to_right_index = player.tile_to_right(tilemap);

    v2d curr_tile_center_pos = tilemap.center_pos_of_tile(tile_curr_index.row, tile_curr_index.col);

    if (VelocityIsUp(player.vel)
        && TilemapIsCollisionTileAt(tilemap, tile_above_index.row, tile_above_index.col)
        && player.local_tilemap_pos.y < curr_tile_center_pos.y) // moving up
    {
        player.local_tilemap_pos.y = curr_tile_center_pos.y;
        player.vel.y = 0.0f;
    }
    if (VelocityIsDown(player.vel)
        && TilemapIsCollisionTileAt(tilemap, tile_below_index.row, tile_below_index.col)
        && player.local_tilemap_pos.y > curr_tile_center_pos.y) // moving down
    {
        player.local_tilemap_pos.y = curr_tile_center_pos.y;
        player.vel.y = 0.0f;
    }
    if (VelocityIsLeft(player.vel)
        && TilemapIsCollisionTileAt(tilemap, tile_to_left_index.row, tile_to_left_index.col)
        && player.local_tilemap_pos.x < curr_tile_center_pos.x) // moving left
    {
        player.local_tilemap_pos.x = curr_tile_center_pos.x;
        player.vel.x = 0.0f;
    }
    if (VelocityIsRight(player.vel)
        && TilemapIsCollisionTileAt(tilemap, tile_to_right_index.row, tile_to_right_index.col)
        && player.local_tilemap_pos.x > curr_tile_center_pos.x) // moving right
    {
        player.local_tilemap_pos.x = curr_tile_center_pos.x;
        player.vel.x = 0.0f;
    }
}


//-----------------------------------------------------------------------------------------------
void
PlayerSetPositionTilemapWrap(Player &player, Tilemap tilemap)
{
    if (player.local_tilemap_pos.x < 0.0f)
    {
        player.local_tilemap_pos.x = tilemap.width();
    }
    else if (player.local_tilemap_pos.x > tilemap.width())
    {
        player.local_tilemap_pos.x = 0.0f;
    }
    if (player.local_tilemap_pos.y < 0.0f)
    {
        player.local_tilemap_pos.y = tilemap.height();
    }
    else if (player.local_tilemap_pos.y > tilemap.height())
    {
        player.local_tilemap_pos.y = 0.0f;
    }
}


//-----------------------------------------------------------------------------------------------
void
PlayerCollectTileBoundPellets(Player &player, TileBoundGoodPelletsPool &pellets_pool, Tilemap &tilemap,
                              MovingPelletsPool &moving_pellets_pool)
{
    if(pellets_pool.is_active_at_tile(tilemap, player.current_tile(tilemap)))
    {
        pellets_pool.set_inactive_at_tile(tilemap, player.current_tile(tilemap));
        v2d center_tile_pos = tilemap.center_pos_of_tile(player.current_tile(tilemap).row, player.current_tile(tilemap).col);
        moving_pellets_pool.CreatePellet(center_tile_pos, (v2d){10.0f, -400.0f});
    }
}


//-----------------------------------------------------------------------------------------------
void
PlayerRenderDebugCurrentRect(Player &player, Tilemap tilemap)
{
    TileIndex player_curr_tile_index = player.current_tile(tilemap);

    SDL_Rect curr_tile_rect = {
            player_curr_tile_index.col * (int) tilemap.tile_size,
            player_curr_tile_index.row * (int) tilemap.tile_size,
            (int) tilemap.tile_size,
            (int) tilemap.tile_size
    };

    SDL_SetRenderDrawColor(Global::renderer, 255, 0, 0, 100);
    SDL_RenderFillRect(Global::renderer, &curr_tile_rect);
}


//-----------------------------------------------------------------------------------------------
void
PlayerRenderDebugPositionAsCircle(Player &player, float tilemap_position_x, float tilemap_position_y)
{
    int global_pos_x = tilemap_position_x + player.local_tilemap_pos.x;
    int global_pos_y = tilemap_position_y + player.local_tilemap_pos.y;
    Util::DrawCircleFill(Global::renderer, global_pos_x, global_pos_y, 2, SDL_Color{255, 255, 255, 255});
}


//-----------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    GameSDLSetup();
    Tilemap tilemap;
    WorldPosition tilemap_position = {0.0f, 0.0f};
    TileBoundGoodPelletsPool tile_bound_pellets_pool;
    MovingPelletsPool moving_pellets_pool;
    LoadFileResult tilemap_load_result = TilemapLoadFromFile("tilemap.json", tilemap);
    LoadFileResult pellets_load_result = TileBoundGoodPelletsPoolLoadFromFile("tileboundgoodpelletspool.json",
                                                                              tile_bound_pellets_pool);
    moving_pellets_pool = MovingPelletsPoolInit(1000);
//    for(int i = 0; i < 20; i++)
//    {
//        moving_pellets_pool.CreatePellet((v2d){0.0, 20.0f * i}, (v2d){20.0f, 0.0f});
//    }

    //player setup
    SpriteSheet player_sprite_sheet = SpriteSheetCreateFromFile("assets/robert-anim.png", "player", 1, 12);
    AnimatedSprite player_animated_sprite = AnimatedSpriteInit(player_sprite_sheet, 0, 11, 12);
    Player player = PlayerInit(player_animated_sprite, 0.0f, -22.0f, 10, 10, 2.0f, tilemap);


    double delta_time_in_seconds;
    double frame_start_seconds = 0.0f;
    double frame_end_seconds = 0.0f;


    bool quit = false;
    while (!quit)
    {
        //
        // DELTA TIME FRAME BEGIN
        //
        delta_time_in_seconds = frame_end_seconds - frame_start_seconds;
        frame_start_seconds = (double) SDL_GetTicks() / 1000.0f;

        //
        // I/O EVENTS
        //
        while (SDL_PollEvent(&Global::event))
        {
            if (Global::event.type == SDL_QUIT)
            {
                quit = true;
            }

        }

        //
        // INPUT GATHER AND GENERAL HANDLING
        //
        const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
        if (keyboard_state[SDL_SCANCODE_ESCAPE])
        {
            quit = true;
        }
        if (keyboard_state[SDL_SCANCODE_D])
        {
            DEBUG_BREAK;
        }

        //
        // GAMEPLAY
        //
        PlayerSetVelocityAndSetSnapAxisBasedOnInputAndSpeed(player, tilemap, keyboard_state);
        PlayerSetVelocityToSnapToAxis(player, tilemap);
        PlayerMove(player);
        PlayerSetPositionTilemapWrap(player, tilemap);
        PlayerTilemapCollisionHandle(player, tilemap);
        PlayerSetPositionAndSetVelocityOnceFullySnappedOnAxis(player, tilemap);
        player.animated_sprite.increment(delta_time_in_seconds);
        PlayerCollectTileBoundPellets(player, tile_bound_pellets_pool, tilemap, moving_pellets_pool);
        moving_pellets_pool.move_all(delta_time_in_seconds);

        //
        // RENDER
        //
        SDL_SetRenderDrawColor(Global::renderer, 50, 50, 50, 255);
        SDL_RenderClear(Global::renderer);

        TilemapCollisionTileRectsRender(tilemap,
                                        tilemap_position.x,
                                        tilemap_position.y,
                                        SDL_Color{0, 100, 0, 255});
        TilemapGridRender(tilemap,
                          tilemap_position.x,
                          tilemap_position.y,
                          SDL_Color{100, 100, 100, 255});

        TileBoundGoodPelletsPoolRender(tile_bound_pellets_pool, tilemap, (v2d) {tilemap_position.x, tilemap_position.y});

        MovingPelletsPoolRender(moving_pellets_pool, tilemap, (v2d){tilemap_position.x, tilemap_position.y});


        //PlayerRenderDebugCurrentRect(player, tilemap);



        float scale = (tilemap.tile_size * 2.0f) /
                      player.animated_sprite.sprite_sheet.cell_width(); // how big should sprite be relative to tile

        SpriteRender(player.animated_sprite.sprite_sheet, 0, player.animated_sprite.curr_frame, scale,
                     tilemap_position.x, tilemap_position.y, player.local_tilemap_pos.x,
                     player.local_tilemap_pos.y, player.sprite_pos.x, player.sprite_pos.y,
                     player.animated_sprite.flip_horizonatally);

        PlayerRenderDebugPositionAsCircle(player, tilemap_position.x, tilemap_position.y);

        SDL_RenderPresent(Global::renderer);

        //
        // DELTA TIME FRAME END
        //
        frame_end_seconds = (double) SDL_GetTicks() / 1000.0f;
    }
    SDL_DisplayMode displayMode;
    int success = SDL_GetWindowDisplayMode(Global::window, &displayMode);
    displayMode.w = 2560;
    displayMode.h = 1440;
    SDL_SetWindowDisplayMode(Global::window, &displayMode);
    return 0;
}