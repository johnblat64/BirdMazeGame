#include <SDL2/SDL.h>
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_sdlrenderer.h"
#include "flecs/flecs.h"
#include "../src/engine/tile/tilemap.h"
#include "../src/components/components.h"
#include "../src/engine/util/util_draw.h"


//--------------------------------------------------------
SDL_Window *window;
int window_w = 1920;
int window_h = 1080;
float aspect_ratio = 1.77777f;
SDL_Renderer *renderer;
SDL_Event event;
bool should_quit = false;


//--------------------------------------------------------
void 
Game_SDL_Setup()
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(
        "Bird Maze Game", 
        0, 
        0, 
        window_w, 
        window_h, 
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    renderer = SDL_CreateRenderer(
        window, 
        -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
}


//--------------------------------------------------------
void 
Game_ImGui_Setup()
{
    // IMGUI Setup
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);
}


//--------------------------------------------------------
int 
main()
{
    AutoTiledTileMap auto_tile_map = AutoTiledTileMap_init("my sheet", 30, 34, 28);
    WorldPosition tilemap_position = {50.0, 0.0};

    Game_SDL_Setup();
    // SDL_Rect viewport = {0,0,1920, 1080};
    // SDL_RenderSetViewport(renderer,&viewport);
    Game_ImGui_Setup();
    
    while(!should_quit)
    {
        float new_window_w, new_window_h;
        bool window_resized = false;

        while(SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if(event.type == SDL_QUIT)
            {
                exit(0);
            }
            // if( event.type == SDL_WINDOWEVENT ) {
            //     if( event.window.event == SDL_WINDOWEVENT_RESIZED ) {
            //         new_window_w = event.window.data1;
            //         new_window_h = event.window.data2;
            //         new_window_h = new_window_w / aspect_ratio;
            //         SDL_SetWindowSize(window, new_window_w, new_window_h);
            //         float ratio = (float)((float)new_window_w/(float)window_w);
            //         SDL_RenderSetScale(renderer, ratio, ratio);
            //         window_resized = true;
            //     }
            // }
        }

        SDL_SetWindowSize(window, new_window_w, new_window_h);

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Auto Tiler");
        ImGui::Text("Tile Map Properties");
        ImGui::DragFloat2("Offset Position", (float *)&tilemap_position);
        ImGui::InputInt("Tile Size", (int *)&auto_tile_map.tile_size);
        static Uint32 new_n_rows, new_n_cols;
        ImGui::InputInt("Num Rows", (int *)&new_n_rows);
        ImGui::InputInt("Num Cols", (int *)&new_n_cols);
        ImGui::Button("Update Row/Col Dimensions");
        ImGui::Button("Reset New Row/Col to Current");

        ImGui::End();
        ImGui::Render();




        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);

        util_draw_grid(
            renderer, 
            auto_tile_map.tile_size, 
            tilemap_position.x, 
            tilemap_position.y, 
            auto_tile_map.n_rows, 
            auto_tile_map.n_cols
        );

        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);

    }

}