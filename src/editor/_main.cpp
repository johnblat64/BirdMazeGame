#include <external/SDL2/include/SDL.h>
#include <external/imgui/imgui_internal.h>
#include <external/imgui/imgui.h>
#include <external/imgui/backends/imgui_impl_sdl.h>
#include <external/imgui/backends/imgui_impl_sdlrenderer.h>
#include <src/tile/tilemap.h>
#include <src/pellet_pools/tile_bound_good_pellets_pool.h>
#include <src/util/util_draw.h>
#include <src/util/util_load_save.h>
#include <src/util/util_error_handling.h>
#include <src/global.h>
#include <src/editor/editor.h>
#include <src/editor/level_data.h>

#include <iostream>
#ifdef WIN32
#include <windows.h>
#include <direct.h>
#define GetCurrentDirectory _getcwd
#else
#include <unistd.h>
#define GetCurrentDirectory getcwd
#endif
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
    char *whatever = GetCurrentDirectory(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    std::cout << "Current Working Directory: " << current_working_dir << std::endl;
}



//---------------------------------------------------------
int
main(int argc, char *argv[])
{
    PrintCurrentDirectory();

    LevelData level_data;
    LevelDataLoadFromFiles(level_data);

    EditorSDLSetup();
    EditorImGuiSetup();
    Editor::Setup(level_data.tilemap);


    while (!should_quit)
    {
        while (SDL_PollEvent(&Global::event))
        {
            ImGui_ImplSDL2_ProcessEvent(&Global::event);

            if (Global::event.type == SDL_QUIT)
            {
                exit(0);
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

        Editor::EditorWindow(level_data);


        Util::RenderTargetSet(Global::renderer, NULL);


        //Render to screen
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(Global::renderer);
    }
}
