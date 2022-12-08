#include <external/SDL2/include/SDL.h>
#include "imgui_internal.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_sdlrenderer.h"
#include <src/tile/tilemap.h>
#include <src/pellet_pools/tile_bound_good_pellets_pool.h>
#include <src/util/util_draw.h>
#include <src/util/util_load_save.h>
#include <src/util/util_error_handling.h>
#include "src/global.h"
#include "editor.h"
#include <iostream>

#include <windows.h>
#include <direct.h>


//--------------------------------------------------------



bool should_quit = false;



//--------------------------------------------------------
void
EditorSDLSetup()
{
    SDLErrorHandle(SDL_Init(SDL_INIT_VIDEO));
    //SDLErrorHandle(SDL_SetError("This is a test!"));
    Global::window = SDL_CreateWindow(
            "Bird Maze Game",
            0,
            0,
            1920,
            1080,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
    );
    SDLErrorHandleNull(Global::window);
    Global::renderer = SDL_CreateRenderer(
            Global::window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    SDLErrorHandleNull(Global::renderer);
    SDLErrorHandle(SDL_SetRenderDrawBlendMode(Global::renderer, SDL_BLENDMODE_BLEND));

}


//--------------------------------------------------------
void
EditorImGuiSetup()
{
    // IMGUI Setup
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; 
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(Global::window, Global::renderer);
    ImGui_ImplSDLRenderer_Init(Global::renderer);
}




void PrintCurrentDirectory()
{
    char buff[FILENAME_MAX];//create string buffer to hold path
    char *whatever = _getcwd(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    std::cout << current_working_dir << std::endl;
}


//---------------------------------------------------------
int
main(int argc, char *argv[])
{

    PrintCurrentDirectory();
    Tilemap tilemap;
    Tileset tileset;
    TileBoundGoodPelletsPool pellets_pool;
    LoadFileResult load_tilemap_result = TilemapLoadFromFile("assets/tilemap.json", tilemap);
    LoadFileResult load_tileset_result = TilesetLoadFromFile("assets/tileset.json", tileset);

    if (load_tilemap_result == LOAD_FILE_NOT_FOUND)
    {
        // just initialize it to something
        tilemap = Tilemap_init(30, 25, 25);
    }
    else if (load_tilemap_result == LOAD_FULL_BYTES_NOT_READ)
    {
        exit(EXIT_FAILURE);
    }

    if (load_tileset_result == LOAD_FILE_NOT_FOUND)
    {
        tileset = TilesetInit(8, 20);
    }
    else if (load_tileset_result == LOAD_FULL_BYTES_NOT_READ)
    {
        exit(EXIT_FAILURE);
    }

    LoadFileResult pellets_pool_load_result = TileBoundGoodPelletsPoolLoadFromFile("assets/tileboundgoodpelletspool.json", pellets_pool);

    if (pellets_pool_load_result == LOAD_FILE_NOT_FOUND)
    {
        // just initialize it to something
        pellets_pool = TileBoundGoodPelletsPoolInit(tilemap.n_tiles());
    }
    else if (pellets_pool_load_result == LOAD_FULL_BYTES_NOT_READ)
    {
        exit(EXIT_FAILURE);
    }

    Uint32 delta_time_frame_start_ticks = 0;
    Uint32 delta_time_frame_end_ticks = 0;


    EditorSDLSetup();
    EditorImGuiSetup();
    Editor::Setup(tilemap);

    float render_ratio = 1.0f;



    while (!should_quit)
    {

        Global::delta_time_ms = delta_time_frame_end_ticks - delta_time_frame_start_ticks;

        delta_time_frame_start_ticks = SDL_GetTicks();

        float new_window_w;

        while (SDL_PollEvent(&Global::event))
        {
            ImGui_ImplSDL2_ProcessEvent(&Global::event);

            if (Global::event.type == SDL_QUIT)
            {
                exit(0);
            }
            if (Global::event.type == SDL_WINDOWEVENT)
            {
                if (Global::event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    new_window_w = (float) Global::event.window.data1;
                    render_ratio = (float) ((float) new_window_w / (float) Global::window_w);
                }
            }
            if (Global::event.type == SDL_KEYDOWN)
            {
                if (Global::event.key.keysym.scancode == SDL_SCANCODE_F11)
                {
                    SDL_SetWindowFullscreen(Global::window, 0);
                }
            }
        }

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        Editor::EditorWindow(tilemap, tileset, pellets_pool);


        Util::RenderTargetSet(Global::renderer, NULL);


        //Render to screen
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(Global::renderer);

        delta_time_frame_end_ticks = SDL_GetTicks();
    }

}
