#include <SDL.h>
#include <src/engine/util/util_error_handling.h>
#include <src/engine/global.h>
#include <src/engine/sprite/sprite_sheet.h>
#include "src/engine/tile/tilemap.h"
#include "src/engine/util/util_load_save.h"


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
    float pos_x; float pos_y;
    float vel_x; float vel_y;
    float speed;
};


int
main()
{
    GameSDLSetup();
    Tilemap tilemap;
    WorldPosition tilemap_position = {0.0f, 0.0f};
    LoadFileResult load_result = Tilemap_load_from_file("tilemap.json", tilemap);

    //player setup
    Player player;
    player.sprite_sheet = SpriteSheetCreateFromFile("assets/robert.png", "player", 1, 1);
    player.pos_x = 0.0f;
    player.pos_y = 0.0f;
    player.vel_x = 0.0f;
    player.vel_y = 0.0f;
    player.speed = 5.0f;


    bool quit = false;
    while(!quit)
    {
        while(SDL_PollEvent(&Global::event))
        {
            if(Global::event.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
        if(keyboard_state[SDL_SCANCODE_UP])
        {
            player.vel_y = -player.speed;
            player.vel_x = 0.0f;
        }
        else if(keyboard_state[SDL_SCANCODE_DOWN])
        {
            player.vel_y = player.speed;
            player.vel_x = 0.0f;
        }
        else if(keyboard_state[SDL_SCANCODE_LEFT])
        {
            player.vel_x = -player.speed;
            player.vel_y = 0.0f;
        }
        else if(keyboard_state[SDL_SCANCODE_RIGHT])
        {
            player.vel_x = player.speed;
            player.vel_y = 0.0f;
        }

        player.pos_x += player.vel_x;
        player.pos_y += player.vel_y;

        SDL_SetRenderDrawColor(Global::renderer, 50,50,50,255);
        SDL_RenderClear(Global::renderer);

        TilemapCollisionTileRectsRender(tilemap,
                                        tilemap_position.x,
                                        tilemap_position.y,
                                        (SDL_Color) {255, 0, 0, 255});
        TilemapGridRender(tilemap,
                          tilemap_position.x,
                          tilemap_position.y,
                          (SDL_Color) {100, 100, 100, 255});

        SDL_RenderPresent(Global::renderer);
    }
    return 0;
}