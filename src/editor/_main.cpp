#include <SDL.h>
#include "imgui_internal.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_sdlrenderer.h"
#include "src/engine/tile/tilemap.h"
#include "src/engine/util/util_draw.h"
#include "src/engine/util/util_load_save.h"
#include "src/engine/util/util_error_handling.h"
#include <src/engine/global.h>
#include "editor.h"

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


//--------------------------------------------------------
void
EditorImGuiSetup()
{
    // IMGUI Setup
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(Global::window, Global::renderer);
    ImGui_ImplSDLRenderer_Init(Global::renderer);
}


//---------------------------------------------------------
int
main(int argc, char *argv[])
{
    Tilemap tilemap;
    LoadFileResult load_result = Tilemap_load_from_file("tilemap.json", tilemap);

    if (load_result == LOAD_FILE_NOT_FOUND)
    {
        // just initialize it to something
        tilemap = Tilemap_init(30, 25, 25);
    }
    else if (load_result == LOAD_FULL_BYTES_NOT_READ)
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
        }

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        Editor::EditorWindow(tilemap);


        Util::RenderTargetSet(Global::renderer, NULL);


        //Render to screen
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(Global::renderer);

        delta_time_frame_end_ticks = SDL_GetTicks();
    }

}
