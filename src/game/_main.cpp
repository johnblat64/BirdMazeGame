#include <SDL.h>
#include <src/engine/util/util_error_handling.h>
#include <src/engine/global.h>
#include <src/engine/sprite/sprite_sheet.h>
#include "src/engine/tile/tilemap.h"
#include "src/engine/util/util_load_save.h"
#define STB_IMAGE_IMPLEMENTATION
#include <external/stb/stb_image.h>
#include <src/engine/util/util_draw.h>
#include <math.h>


int sign(float x)
{
    return x >= 0.0 ? 1 : -1;
}


double degrees_to_rads(double degrees)
{
    return  degrees * 3.14f / 180.0f;
}


void
GameSDLSetup()
{
    SDLErrorHandle(SDL_Init(SDL_INIT_VIDEO));
    //SDLErrorHandle(SDL_SetError("This is a test!"));
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

enum Axis
{
    AXIS_X,
    AXIS_Y
};

struct Player
{
    AnimatedSprite animated_sprite;
    float sprite_pos_x;
    float sprite_pos_y; // local to player
    float tilemap_pos_x;
    float tilemap_pos_y; // local to tilemap
    float vel_x;
    float vel_y;
    float speed;
    Axis axis;


    void current_tile(Tilemap tilemap, int &tile_x, int &tile_y)
    {
        tile_x = (int)tilemap_pos_x  / tilemap.tile_size;
        tile_y = (int)tilemap_pos_y / tilemap.tile_size;
    }

    void tile_above(Tilemap tilemap, int &tile_x, int &tile_y)
    {
        current_tile(tilemap, tile_x, tile_y);
        tile_y--;
    }

    void tile_below(Tilemap tilemap, int &tile_x, int &tile_y)
    {
        current_tile(tilemap, tile_x, tile_y);
        tile_y++;
    }

    void tile_to_left(Tilemap tilemap, int &tile_x, int &tile_y)
    {
        current_tile(tilemap, tile_x, tile_y);
        tile_x--;
    }

    void tile_to_right(Tilemap tilemap, int &tile_x, int &tile_y)
    {
        current_tile(tilemap, tile_x, tile_y);
        tile_x++;
    }
};


int
main()
{
    GameSDLSetup();
    Tilemap tilemap;
    WorldPosition tilemap_position = {0.0f, 0.0f};
    LoadFileResult load_result = Tilemap_load_from_file("tilemap.json", tilemap);

    int player_start_tile_x = 10;
    int player_start_tile_y = 10;
    //player setup
    Player player;
    player.animated_sprite.sprite_sheet = SpriteSheetCreateFromFile("assets/robert-anim.png", "player", 1, 12);
    player.animated_sprite.seconds_per_frame = 0.06f;
    player.animated_sprite.accumulator = 0.0f;
    player.animated_sprite.start_frame = 0;
    player.animated_sprite.end_frame = 11;
    player.vel_x = 0.0f;
    player.vel_y = 0.0f;
    player.speed = 0.5f;
    player.sprite_pos_x = 0.0f;
    player.sprite_pos_y = -20.0f;


    tilemap.center_pt_of_tile(player_start_tile_y, player_start_tile_x, player.tilemap_pos_x, player.tilemap_pos_y);
    double delta_time_in_seconds;
    double frame_start_seconds = 0.0f;
    double frame_end_seconds = 0.0f;

    bool flip;

    bool quit = false;
    while (!quit)
    {
        delta_time_in_seconds = frame_end_seconds - frame_start_seconds;
        frame_start_seconds = (double) SDL_GetTicks() / 1000.0f;

        while (SDL_PollEvent(&Global::event))
        {
            if (Global::event.type == SDL_QUIT)
            {
                quit = true;
            }

        }


        const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
        if(keyboard_state[SDL_SCANCODE_ESCAPE])
        {
            quit = true;
        }

        //
        // set vel
        //
        int player_tile_above_x, player_tile_above_y;
        int player_tile_below_x, player_tile_below_y;
        int player_tile_to_left_x, player_tile_to_left_y;
        int player_tile_to_right_x, player_tile_to_right_y;
        player.tile_above(tilemap, player_tile_above_x, player_tile_above_y);
        player.tile_below(tilemap, player_tile_below_x, player_tile_below_y);
        player.tile_to_left(tilemap, player_tile_to_left_x, player_tile_to_left_y);
        player.tile_to_right(tilemap, player_tile_to_right_x, player_tile_to_right_y);

        if (keyboard_state[SDL_SCANCODE_UP] &&
                std_vector_2d_at<char>(tilemap.is_collision_tiles, player_tile_above_y, player_tile_above_x,
                                       tilemap.n_cols) == false)
        {
            player.vel_y = -player.speed;
            player.vel_x = 0.0f;
            player.axis = AXIS_Y;
        }
        else if (keyboard_state[SDL_SCANCODE_DOWN] &&
                std_vector_2d_at<char>(tilemap.is_collision_tiles, player_tile_below_y, player_tile_below_x,
                                       tilemap.n_cols) == false)
        {
            player.vel_y = player.speed;
            player.vel_x = 0.0f;
            player.axis = AXIS_Y;
        }
        else if (keyboard_state[SDL_SCANCODE_LEFT] &&
                std_vector_2d_at<char>(tilemap.is_collision_tiles, player_tile_to_left_y, player_tile_to_left_x,
                                       tilemap.n_cols) == false)
        {
            player.vel_x = -player.speed;
            player.vel_y = 0.0f;
            flip = false;
            player.axis = AXIS_X;
        }
        else if (keyboard_state[SDL_SCANCODE_RIGHT] &&
                std_vector_2d_at<char>(tilemap.is_collision_tiles, player_tile_to_right_y, player_tile_to_right_x,
                                       tilemap.n_cols) == false)
        {
            player.vel_x = player.speed;
            player.vel_y = 0.0f;
            flip = true;
            player.axis = AXIS_X;
        }

        //
        // adjust vel if not along center tile axis
        //
        if(player.vel_y == -player.speed) // moving up
        {
            int curr_tile_x, curr_tile_y;
            player.current_tile(tilemap, curr_tile_x, curr_tile_y);
            float curr_tile_center_x, curr_tile_center_y;
            tilemap.center_pt_of_tile(curr_tile_x, curr_tile_y, curr_tile_center_x, curr_tile_center_y);

            if(player.tilemap_pos_x > curr_tile_center_x) // to right of tile center y-axis
            {
                player.vel_y = -0.2f;
                player.vel_x = -0.2f;
            }
            else if(player.tilemap_pos_x < curr_tile_center_x) // to right of tile center y-axis
            {
                player.vel_y = -0.2f;
                player.vel_x = 0.2f;
            }
        }
        if(player.vel_y == player.speed) // moving down
        {
            int curr_tile_x, curr_tile_y;
            player.current_tile(tilemap, curr_tile_x, curr_tile_y);
            float curr_tile_center_x, curr_tile_center_y;
            tilemap.center_pt_of_tile(curr_tile_x, curr_tile_y, curr_tile_center_x, curr_tile_center_y);

            if(player.tilemap_pos_x > curr_tile_center_x) // to right of tile center y-axis
            {
                player.vel_y = 0.2f;
                player.vel_x = -0.2f;
            }
            else if(player.tilemap_pos_x < curr_tile_center_x) // to right of tile center y-axis
            {
                player.vel_y = 0.2f;
                player.vel_x = 0.2f;
            }
        }
        if(player.vel_x == -player.speed) // moving left
        {
            int curr_tile_x, curr_tile_y;
            player.current_tile(tilemap, curr_tile_x, curr_tile_y);
            float curr_tile_center_x, curr_tile_center_y;
            tilemap.center_pt_of_tile(curr_tile_x, curr_tile_y, curr_tile_center_x, curr_tile_center_y);

            if(player.tilemap_pos_y > curr_tile_center_y) // to bottom of tile center x-axis
            {
                player.vel_y = -0.2f;
                player.vel_x = -0.2f;
            }
            else if(player.tilemap_pos_y < curr_tile_center_y) // to top of tile center x-axis
            {
                player.vel_y = 0.2f;
                player.vel_x = -0.2f;
            }
        }
        if(player.vel_x == player.speed) // moving right
        {
            int curr_tile_x, curr_tile_y;
            player.current_tile(tilemap, curr_tile_x, curr_tile_y);
            float curr_tile_center_x, curr_tile_center_y;
            tilemap.center_pt_of_tile(curr_tile_x, curr_tile_y, curr_tile_center_x, curr_tile_center_y);

            if(player.tilemap_pos_y > curr_tile_center_y) // to bottom of tile center x-axis
            {
                player.vel_y = -0.2f;
                player.vel_x = 0.2f;
            }
            else if(player.tilemap_pos_y < curr_tile_center_y) // to top of tile center x-axis
            {
                player.vel_y = 0.2f;
                player.vel_x = 0.2f;
            }
        }

        //
        // move player
        //
        player.tilemap_pos_x += player.vel_x;
        player.tilemap_pos_y += player.vel_y;

        //
        // check for alignment
        //

        if(player.vel_x != 0.0f && player.axis == AXIS_Y)
        {
            int curr_tile_x, curr_tile_y;
            player.current_tile(tilemap, curr_tile_x, curr_tile_y);
            float curr_tile_center_x, curr_tile_center_y;
            tilemap.center_pt_of_tile(curr_tile_x, curr_tile_y, curr_tile_center_x, curr_tile_center_y);

            if(player.vel_x > 0.0f)
            {
                if(player.tilemap_pos_x > curr_tile_center_x)
                {
                    player.tilemap_pos_x = curr_tile_center_x;
                    player.vel_x = 0.0f;
                    player.vel_y = sign(player.vel_y) * player.speed;
                }
            }
            else
            {
                if(player.tilemap_pos_x < curr_tile_center_x)
                {
                    player.tilemap_pos_x = curr_tile_center_x;
                    player.vel_x = 0.0f;
                    player.vel_y = sign(player.vel_y) * player.speed;
                }
            }
        }
        if(player.vel_y != 0.0f && player.axis == AXIS_X)
        {
            int curr_tile_x, curr_tile_y;
            player.current_tile(tilemap, curr_tile_x, curr_tile_y);
            float curr_tile_center_x, curr_tile_center_y;
            tilemap.center_pt_of_tile(curr_tile_x, curr_tile_y, curr_tile_center_x, curr_tile_center_y);

            if(player.vel_y > 0.0f)
            {
                if(player.tilemap_pos_y > curr_tile_center_y)
                {
                    player.tilemap_pos_y = curr_tile_center_y;
                    player.vel_y = 0.0f;
                    player.vel_x = sign(player.vel_x) * player.speed;
                }
            }
            else
            {
                if(player.tilemap_pos_y < curr_tile_center_y)
                {
                    player.tilemap_pos_y = curr_tile_center_y;
                    player.vel_y = 0.0f;
                    player.vel_x = sign(player.vel_x) * player.speed;
                }
            }
        }


        //
        // collision check
        //
        int player_curr_tile_x, player_curr_tile_y;
        player.current_tile(tilemap, player_curr_tile_x, player_curr_tile_y);

        if(std_vector_2d_at<char>(tilemap.is_collision_tiles, player_curr_tile_y, player_curr_tile_x, tilemap.n_cols) == true)
        {
            float vel_x = -player.vel_x;
            float vel_y = -player.vel_y;
            player.tilemap_pos_x += vel_x;
            player.tilemap_pos_y += vel_y;
            player.vel_x = 0.0f;
            player.vel_y = 0.0f;
        }

        if(player.tilemap_pos_x < 0.0f)
        {
            player.tilemap_pos_x = tilemap.width();
        }
        else if(player.tilemap_pos_x > tilemap.width())
        {
            player.tilemap_pos_x = 0.0f;
        }
        if(player.tilemap_pos_y < 0.0f)
        {
            player.tilemap_pos_y = tilemap.height();
        }
        else if(player.tilemap_pos_y > tilemap.height())
        {
            player.tilemap_pos_y = 0.0f;
        }

        SDL_SetRenderDrawColor(Global::renderer, 50, 50, 50, 255);
        SDL_RenderClear(Global::renderer);

        TilemapCollisionTileRectsRender(tilemap,
                                        tilemap_position.x,
                                        tilemap_position.y,
                                        (SDL_Color) {0, 100, 0, 255});
        TilemapGridRender(tilemap,
                          tilemap_position.x,
                          tilemap_position.y,
                          (SDL_Color) {100, 100, 100, 255});

        player.current_tile(tilemap, player_curr_tile_x, player_curr_tile_y);

        SDL_Rect curr_tile_rect = {
                player_curr_tile_x * (int)tilemap.tile_size,
                player_curr_tile_y * (int)tilemap.tile_size,
                (int)tilemap.tile_size,
                (int)tilemap.tile_size
        };

        SDL_SetRenderDrawColor(Global::renderer, 255,0,0,100);
        SDL_RenderFillRect(Global::renderer, &curr_tile_rect);

        // animated sprite update
        player.animated_sprite.increment(delta_time_in_seconds);
        // how big should sprite be relative to tile
        float scale = (tilemap.tile_size * 2.0f) / player.animated_sprite.sprite_sheet.cell_width();

        SpriteRender(player.animated_sprite.sprite_sheet, 0, player.animated_sprite.curr_frame, scale,
                     tilemap_position.x, tilemap_position.y, player.tilemap_pos_x,
                     player.tilemap_pos_y, player.sprite_pos_x, player.sprite_pos_y, flip);

        Util::DrawCircleFill(Global::renderer, tilemap_position.x + player.tilemap_pos_x, tilemap_position.y + player.tilemap_pos_y, 2, (SDL_Color){255, 255, 255, 255});




        SDL_RenderPresent(Global::renderer);

        frame_end_seconds = (double) SDL_GetTicks() / 1000.0f;
    }
    SDL_DisplayMode displayMode;
    int success = SDL_GetWindowDisplayMode(Global::window, &displayMode);
    displayMode.w = 2560;
    displayMode.h = 1440;
    SDL_SetWindowDisplayMode(Global::window, &displayMode);
    return 0;
}