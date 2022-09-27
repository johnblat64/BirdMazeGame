#include <SDL.h>
#include <src/engine/util/util_error_handling.h>
#include <src/engine/global.h>
#include <src/engine/sprite/sprite_sheet.h>
#include "src/engine/tile/tilemap.h"
#include "src/engine/util/util_load_save.h"
#define STB_IMAGE_IMPLEMENTATION
#include <external/stb/stb_image.h>
#include <src/engine/util/util_draw.h>

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


struct Player
{
    SpriteSheet sprite_sheet;
    float sprite_pos_x;
    float sprite_pos_y; // local to player
    float tilemap_pos_x;
    float tilemap_pos_y; // local to tilemap
    float vel_x;
    float vel_y;
    float speed;
};


int
main()
{
    GameSDLSetup();
    Tilemap tilemap;
    WorldPosition tilemap_position = {100.0f, 100.0f};
    LoadFileResult load_result = Tilemap_load_from_file("tilemap.json", tilemap);

    //player setup
    Player player;
    player.sprite_sheet = SpriteSheetCreateFromFile("assets/robert-anim.png", "player", 1, 12);
    player.tilemap_pos_x = 100.0f;
    player.tilemap_pos_y = 100.0f;
    player.vel_x = 0.0f;
    player.vel_y = 0.0f;
    player.speed = 1.9f;
    player.sprite_pos_x = 0.0f;
    player.sprite_pos_y = -20.0f;


    bool quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&Global::event))
        {
            if (Global::event.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
        if (keyboard_state[SDL_SCANCODE_UP])
        {
            player.vel_y = -player.speed;
            player.vel_x = 0.0f;
        }
        else if (keyboard_state[SDL_SCANCODE_DOWN])
        {
            player.vel_y = player.speed;
            player.vel_x = 0.0f;
        }
        else if (keyboard_state[SDL_SCANCODE_LEFT])
        {
            player.vel_x = -player.speed;
            player.vel_y = 0.0f;
        }
        else if (keyboard_state[SDL_SCANCODE_RIGHT])
        {
            player.vel_x = player.speed;
            player.vel_y = 0.0f;
        }

        player.tilemap_pos_x += player.vel_x;
        player.tilemap_pos_y += player.vel_y;
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
                                        (SDL_Color) {255, 0, 0, 255});
        TilemapGridRender(tilemap,
                          tilemap_position.x,
                          tilemap_position.y,
                          (SDL_Color) {100, 100, 100, 255});

        // how big should sprite be relative to tile
        float scale = (tilemap.tile_size * 2.0f) / player.sprite_sheet.cell_width();
        SpriteRender(player.sprite_sheet, 0, 0, 0.5, tilemap_position.x, tilemap_position.y, player.tilemap_pos_x,
                     player.tilemap_pos_y, player.sprite_pos_x, player.sprite_pos_y);
        Util::DrawCircleFill(Global::renderer, tilemap_position.x + player.tilemap_pos_x, tilemap_position.y + player.tilemap_pos_y, 2, (SDL_Color){255, 255, 255, 255});
        SDL_RenderPresent(Global::renderer);
    }
    return 0;
}