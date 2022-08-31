#include <SDL.h>
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
main(int argc, char* argv[])
{
    AutoTiledTileMap auto_tile_map = AutoTiledTileMap_init("my sheet", 30, 34, 28);
    WorldPosition tilemap_position = {0.0, 0.0};
    Uint32 imgui_n_rows = auto_tile_map.n_rows; 
    Uint32 imgui_n_cols = auto_tile_map.n_cols;
    int tilemap_row_mouse_on_display = 0;
    int tilemap_col_mouse_on_display = 0;
    int window_mouse_x = 0;
    int window_mouse_y = 0;
    float logical_mouse_x = 0;
    float logical_mouse_y = 0;
    int relative_tilemap_mouse_x;
    int relative_tilemap_mouse_y;




    Game_SDL_Setup();
    Game_ImGui_Setup();
    
    float render_ratio = 1.0f;

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
            if( event.type == SDL_WINDOWEVENT ) {
                if( event.window.event == SDL_WINDOWEVENT_RESIZED ) {
                    new_window_w = event.window.data1;
                    new_window_h = event.window.data2;
                    new_window_h = new_window_w / aspect_ratio;
                    window_resized = true;
                    render_ratio = (float)((float)new_window_w/(float)window_w);
                    // SDL_RenderSetScale(renderer, render_ratio, render_ratio);
                }
            }
        }

        bool ImGui_was_focused_this_frame = false;
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Auto Tiler");
        ImGui::Text("Tile Map Properties");
        ImGui::DragFloat2("Offset Position", (float *)&tilemap_position);
        ImGui::InputInt("Tile Size", (int *)&auto_tile_map.tile_size);
        ImGui::InputInt("Num Rows", (int *)&imgui_n_rows);
        ImGui::InputInt("Num Cols", (int *)&imgui_n_cols);
        if(ImGui::Button("Update Row/Col Dimensions"))
        { // Use a button so user can confirm this cause it could lose data if subtracting rows/columns
            AutoTiledTileMap_resize_and_shift_values(
                &auto_tile_map,
                imgui_n_rows,
                imgui_n_cols
            );
        }
        if(ImGui::Button("Reset New Row/Col to Current"))
        { // If user wants to reset to the current rows/cols cause they realized they don't wanna change it anymore
            imgui_n_rows = auto_tile_map.n_rows;
            imgui_n_cols = auto_tile_map.n_cols;
        }
        ImGui::Text("Window Mouse Pos:  (%d, %d)", window_mouse_x, window_mouse_y);
        ImGui::Text("Logical Mouse Pos: (%.2f, %.2f)", logical_mouse_x, logical_mouse_y);
        ImGui::Text("TileMap Index:     (%d, %d)", tilemap_row_mouse_on_display, tilemap_col_mouse_on_display);

        ImGui_was_focused_this_frame = ImGui::IsWindowFocused();

        ImGui::End();
        ImGui::Render();


        SDL_RenderSetScale(renderer, render_ratio, render_ratio); // set this here to run game code that's dependent on render scale like getting the logical mouse position

        // need to get logical mouse and relative mouse to determine what tile is being accessed since it won't be aligned completely with the window/renderer
        

        Uint32 mouse_button_state = SDL_GetMouseState(&window_mouse_x, &window_mouse_y);
        SDL_RenderWindowToLogical(
            renderer, 
            window_mouse_x, 
            window_mouse_y, 
            &logical_mouse_x, 
            &logical_mouse_y
        );

        // find out what tile the mouse cursor is in because this info is useful to the user
        relative_tilemap_mouse_x = logical_mouse_x - (int)tilemap_position.x;
        relative_tilemap_mouse_y = logical_mouse_y - (int)tilemap_position.y;
        if(relative_tilemap_mouse_x >= 0 && relative_tilemap_mouse_y >= 0)
        {
            tilemap_row_mouse_on_display = relative_tilemap_mouse_y / auto_tile_map.tile_size;
            tilemap_col_mouse_on_display = relative_tilemap_mouse_x / auto_tile_map.tile_size;

            // if(tilemap_row_mouse_on_display < auto_tile_map.n_rows && tilemap_col_mouse_on_display < auto_tile_map.n_cols)
            // {
            //     tile_selected = true;
            // }
        }

        // if you don't check for Imgui Window focused then if the imgui window is over the tilemap, 
        // clicking on the imgui window and/or widgets then will affect the tilemap too
        // if the user is focused on the imgui window, then we want to ignore checking for tilemap selections
        if(!ImGui_was_focused_this_frame) 
        {
            if(mouse_button_state & SDL_BUTTON_LMASK)
            {
                bool tile_selected = false;
                int row_selected = 0;
                int col_selected = 0;

                relative_tilemap_mouse_x = logical_mouse_x - (int)tilemap_position.x;
                relative_tilemap_mouse_y = logical_mouse_y - (int)tilemap_position.y;

                if(relative_tilemap_mouse_x >= 0 && relative_tilemap_mouse_y >= 0)
                {
                    row_selected = relative_tilemap_mouse_y / auto_tile_map.tile_size;
                    col_selected = relative_tilemap_mouse_x / auto_tile_map.tile_size;

                    if(row_selected < auto_tile_map.n_rows && col_selected < auto_tile_map.n_cols)
                    {
                        tile_selected = true;
                    }
                }

                if(tile_selected)
                {
                    AutoTiledTileMap_set_wall_value(&auto_tile_map, row_selected, col_selected, true);
                }
            }
            else if(mouse_button_state & SDL_BUTTON_RMASK)
            {
                bool tile_selected = false;
                int row_selected = 0;
                int col_selected = 0;

                relative_tilemap_mouse_x = logical_mouse_x - (int)tilemap_position.x;
                relative_tilemap_mouse_y = logical_mouse_y - (int)tilemap_position.y;

                if(relative_tilemap_mouse_x >= 0 && relative_tilemap_mouse_y >= 0)
                {
                    row_selected = relative_tilemap_mouse_y / auto_tile_map.tile_size;
                    col_selected = relative_tilemap_mouse_x / auto_tile_map.tile_size;

                    if(row_selected < auto_tile_map.n_rows && col_selected < auto_tile_map.n_cols)
                    {
                        tile_selected = true;
                    }
                }

                if(tile_selected)
                {
                    AutoTiledTileMap_set_wall_value(&auto_tile_map, row_selected, col_selected, false);
                }
            }
        }

       


        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);

        SDL_Rect mouseRect = {
            (int)logical_mouse_x,
            (int)logical_mouse_y,
            3,
            3
        };

        SDL_Rect relativeMouseRect = {
            (int)relative_tilemap_mouse_x,
            (int)relative_tilemap_mouse_y,
            3,
            3
        };

        SDL_SetRenderDrawColor(renderer, 0,255,255,255);
        SDL_RenderFillRect(renderer, &mouseRect);

        SDL_SetRenderDrawColor(renderer, 0,255,0,255);
        SDL_RenderFillRect(renderer, &relativeMouseRect);

        // render tilemap walls
        for(int row = 0; row < auto_tile_map.n_rows; row++)
        {
            for(int col = 0; col < auto_tile_map.n_cols; col++)
            {
                bool is_wall = AutoTiledTileMap_get_wall_value(auto_tile_map, row, col);
                if(is_wall)
                {
                    SDL_FRect wall_rect = {
                        tilemap_position.x + (col * auto_tile_map.tile_size),
                        tilemap_position.y + (row * auto_tile_map.tile_size),
                        (float)auto_tile_map.tile_size,
                        (float)auto_tile_map.tile_size
                    };

                    SDL_SetRenderDrawColor(renderer, 255,0,0,255);
                    SDL_RenderFillRectF(renderer, &wall_rect);
                }
            }
        }
            
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);

        util_draw_grid(
            renderer, 
            auto_tile_map.tile_size, 
            tilemap_position.x, 
            tilemap_position.y, 
            auto_tile_map.n_rows, 
            auto_tile_map.n_cols
        );

        SDL_RenderSetScale(renderer, 1, 1); // Set render scale back to 1, 1 before running imgui rendering, because imgui should stay the same size regardless of scale of the game world

        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);

    }

}