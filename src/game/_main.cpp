#include <SDL.h>
#include "imgui_internal.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_sdlrenderer.h"
#include "src/engine/tile/tilemap.h"
#include "src/components/components.h"
#include "src/engine/util/util_draw.h"
#include "src/engine/util/util_load_save.h"
#include "src/engine/util/util_error_handling.h"


//--------------------------------------------------------
SDL_Window *window;
int window_w = 1920;
int window_h = 1080;
SDL_Renderer *renderer;
SDL_Event event;
SDL_Texture *texture;
bool should_quit = false;
ImVec2 dock_size{1920, 1080};


//--------------------------------------------------------
void
Game_SDL_Setup()
{
    SDLErrorHandle(SDL_Init(SDL_INIT_VIDEO));
    //SDLErrorHandle(SDL_SetError("This is a test!"));
    window = SDL_CreateWindow(
            "Bird Maze Game",
            0,
            0,
            window_w,
            window_h,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    SDLErrorHandleNull(window);
    renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    SDLErrorHandleNull(renderer);

}


//--------------------------------------------------------
void
Game_ImGui_Setup()
{
    // IMGUI Setup
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);
}


//----------------------------------------------------------
void
Create_Blank_Texture(int width, int height)
{
    //Create uninitialized texture
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    SDLErrorHandleNull(texture);
}


//--------------------------------------------------------
void
Set_Render_Target(SDL_Renderer *renderer, SDL_Texture *texture)
{
    SDLErrorHandle(SDL_SetRenderTarget(renderer, texture));
}


//--------------------------------------------------------
void
Set_Editor_Layout(ImGuiID dockspace_id)
{
    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, dock_size);
    ImGuiID right;
    ImGuiID left;

    ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.3f, &right, &left);
    ImGui::DockBuilderDockWindow("Auto Tiler", right);
    ImGui::DockBuilderDockWindow("Tilemap", left);
    ImGui::DockBuilderDockWindow("Tileset", left);

    // Uncomment this part out if you want to keep the tilemap window fixed in its docked position
    //ImGuiDockNode* node = ImGui::DockBuilderGetNode(left);
    //node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

    ImGui::DockBuilderFinish(dockspace_id);
}


//---------------------------------------------------------
int
main(int argc, char *argv[])
{
    AutoTiledTileMap auto_tile_map;
    auto_tile_map.tile_size = 30;
    LoadFileResult load_result = util_load_AutoTiledTileMap_walls(
            auto_tile_map.walls,
            auto_tile_map.n_rows,
            auto_tile_map.n_cols
    );
    if(load_result == LOAD_FILE_NOT_FOUND)
    {
        // just initialize it to something
        auto_tile_map = AutoTiledTileMap_init("my sheet", 30, 25, 25);
    }
    else if(load_result == LOAD_FULL_BYTES_NOT_READ)
    {
        exit(EXIT_FAILURE);
    }

    WorldPosition tilemap_position = {0.0, 0.0};

    Uint32 imgui_n_rows = auto_tile_map.n_rows;
    Uint32 imgui_n_cols = auto_tile_map.n_cols;
    Uint32 imgui_save_notification_duration_ms = 3000;
    Uint32 imgui_save_notification_timer = 0;
    char *imgui_tilemap_save_notification_text;
    char *imgui_tilemap_save_notification_text_success = (char *) "Saved Successfully!";
    char *imgui_tilemap_save_notification_text_failure = (char *) "Save Failed!!!!";

    Uint32 delta_time_frame_start_ticks = 0;
    Uint32 delta_time_frame_end_ticks = 0;
    Uint32 delta_time_ms;

    int tilemap_row_mouse_on_display = 0;
    int tilemap_col_mouse_on_display = 0;
    int window_mouse_x = 0;
    int window_mouse_y = 0;
    float logical_mouse_x = 0;
    float logical_mouse_y = 0;
    float relative_tilemap_mouse_x;
    float relative_tilemap_mouse_y;

    ImVec2 panel_size = {(float) (auto_tile_map.tile_size * auto_tile_map.n_cols),
                         (float) (auto_tile_map.tile_size * auto_tile_map.n_rows)};
    ImVec2 screen_pos;
    ImGuiID dockspace_id;
    bool firstLoop = true;
    bool TilemapFocus = false;

    Game_SDL_Setup();
    Game_ImGui_Setup();

    float render_ratio = 1.0f;

    Create_Blank_Texture((int) panel_size.x, (int) panel_size.y);
    while (!should_quit)
    {
        delta_time_ms = delta_time_frame_end_ticks - delta_time_frame_start_ticks;

        delta_time_frame_start_ticks = SDL_GetTicks();

        float new_window_w;

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT)
            {
                exit(0);
            }
            if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    new_window_w = (float)event.window.data1;
                    render_ratio = (float)((float)new_window_w / (float)window_w);
                }
            }
        }

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        TilemapFocus = false;
        static bool dockSpaceOpen = true;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // Most of this code is just setting the flags for the style and layout of the docking space.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("AutoTile Editor", &dockSpaceOpen, window_flags);
        ImGui::PopStyleVar(3);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        if (firstLoop)
        {
            //ImGui::DockBuilderRemoveNode(dockspace_id);
            Set_Editor_Layout(dockspace_id);
            firstLoop = false;
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Close", NULL, false))
                {
                    dockSpaceOpen = false;
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
        // Auto Tile Menu
        if (ImGui::Begin("Auto Tiler"))
        {
            ImGui::Text("Tile Map Properties");
            ImGui::DragFloat2("Offset Position", (float*)&tilemap_position);
            ImGui::InputInt("Tile Size", (int*)&auto_tile_map.tile_size);
            ImGui::InputInt("Num Rows", (int*)&imgui_n_rows);
            ImGui::InputInt("Num Cols", (int*)&imgui_n_cols);
            if (ImGui::Button("Update Row/Col Dimensions"))
            { // Use a button so user can confirm this because it could lose data if subtracting rows/columns
                AutoTiledTileMap_resize_and_shift_values(
                    &auto_tile_map,
                    imgui_n_rows,
                    imgui_n_cols
                );
            }
            if (ImGui::Button("Reset New Row/Col to Current"))
            { // If user wants to reset to the current rows/cols because they realized they don't want to change it anymore
                imgui_n_rows = auto_tile_map.n_rows;
                imgui_n_cols = auto_tile_map.n_cols;
            }
            ImGui::Text("Window Mouse Pos:  (%d, %d)", window_mouse_x, window_mouse_y);
            ImGui::Text("Logical Mouse Pos: (%.2f, %.2f)", logical_mouse_x, logical_mouse_y);
            ImGui::Text("TileMap Index:     (%d, %d)", tilemap_row_mouse_on_display, tilemap_col_mouse_on_display);

            if (ImGui::Button("Save TileMap"))
            {
                bool success = util_save_AutoTiledTileMap_walls(
                    auto_tile_map.walls,
                    auto_tile_map.n_rows,
                    auto_tile_map.n_cols
                );
                if (success)
                {
                    imgui_tilemap_save_notification_text = imgui_tilemap_save_notification_text_success;
                }
                else
                {
                    imgui_tilemap_save_notification_text = imgui_tilemap_save_notification_text_failure;
                }
                imgui_save_notification_timer = imgui_save_notification_duration_ms;
            }

            if (imgui_save_notification_timer > 0)
            {
                imgui_save_notification_timer -= delta_time_ms;
                ImGui::Text("%s\n", imgui_tilemap_save_notification_text);
            }

            bool ImGui_was_focused_this_frame = ImGui::IsWindowFocused();

            
        }
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        
        if (ImGui::Begin("Tilemap"))
        {
            TilemapFocus = true;
            ImVec2 viewPortPanelSize = ImGui::GetContentRegionAvail();
            if (panel_size.x != viewPortPanelSize.x && panel_size.y != viewPortPanelSize.y)
            {
                SDL_DestroyTexture(texture);
                Set_Render_Target(renderer, NULL);
                Create_Blank_Texture((int)viewPortPanelSize.x, (int)viewPortPanelSize.y);
                Set_Render_Target(renderer, texture);
                panel_size = { viewPortPanelSize.x, viewPortPanelSize.y };
            }
            ImGui::Image(texture, panel_size);
            screen_pos = ImGui::GetCursorScreenPos();
       }
       ImGui::End();

       if (ImGui::Begin("Tileset"))
       {
           ImGui::Text("Test");
           
       }
       ImGui::End();

       ImGui::PopStyleVar();
       ImGui::End();

        //SDLErrorHandle(SDL_RenderSetScale(renderer, render_ratio, render_ratio)); // set this here to run game code that's dependent on render scale like getting the logical mouse position

        // need to get logical mouse and relative mouse to determine what tile is being accessed since it won't be aligned completely with the window/renderer


        Uint32 mouse_button_state = SDL_GetMouseState(&window_mouse_x, &window_mouse_y);

        // We calculate the WORLD(imgui window) to SCREEN position using the following formula.
        // nScreenX = fWorldX - offSetX
        // nScreenX = fWorldY - offSetY
        // Where in our case our world to screen should be 1 to 1. i.e. the world is being rendered at (fWorldX = 0, fWorldY = 0) relative to its display
        // along with our screen starting from (0,0), hence the reason fWorldX and fWorldY do not appear below. 
        window_mouse_x -= (int) screen_pos.x;

        // Since imgui has the convention of its screen position being measure from the BOTTOM LEFT CORNER rather than the TOP LEFT CORNER as one would usually be used to
        // we need to add the panel size to the screen position to get the offset we would normally use for our formula
        // i.e. we are transforming the y coodinate from bottomleft -> topleft.
        window_mouse_y -= ((int) screen_pos.y - (int) panel_size.y);
        SDL_RenderWindowToLogical(
                renderer,
                window_mouse_x,
                window_mouse_y,
                &logical_mouse_x,
                &logical_mouse_y
        );

        // find out what tile the mouse cursor is in because this info is useful to the user
        relative_tilemap_mouse_x = logical_mouse_x - tilemap_position.x;
        relative_tilemap_mouse_y = logical_mouse_y - tilemap_position.y;
        if(relative_tilemap_mouse_x >= 0 && relative_tilemap_mouse_y >= 0)
        {
            tilemap_row_mouse_on_display = static_cast<int>(relative_tilemap_mouse_y / (float) auto_tile_map.tile_size);
            tilemap_col_mouse_on_display = static_cast<int>(relative_tilemap_mouse_x / (float) auto_tile_map.tile_size);
        }

        // if you don't check for Imgui Window focused then if the imgui window is over the tilemap,
        // clicking on the imgui window and/or widgets then will affect the tilemap too
        // if the user is focused on the imgui window, then we want to ignore checking for tilemap selections
        if(TilemapFocus)
        {
            if(mouse_button_state & SDL_BUTTON_LMASK)
            {
                bool tile_selected = false;
                int row_selected;
                int col_selected;

                relative_tilemap_mouse_x = logical_mouse_x - tilemap_position.x;
                relative_tilemap_mouse_y = logical_mouse_y - tilemap_position.y;

                if(relative_tilemap_mouse_x >= 0 && relative_tilemap_mouse_y >= 0)
                {
                    row_selected = static_cast<int>(round(relative_tilemap_mouse_y)) / auto_tile_map.tile_size;
                    col_selected = static_cast<int>(round(relative_tilemap_mouse_x)) / auto_tile_map.tile_size;

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
                int row_selected;
                int col_selected;

                relative_tilemap_mouse_x = logical_mouse_x - tilemap_position.x;
                relative_tilemap_mouse_y = logical_mouse_y - tilemap_position.y;

                if(relative_tilemap_mouse_x >= 0 && relative_tilemap_mouse_y >= 0)
                {
                    row_selected = static_cast<int>(relative_tilemap_mouse_y / (float) auto_tile_map.tile_size);
                    col_selected = static_cast<int>(relative_tilemap_mouse_x / (float) auto_tile_map.tile_size);

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
        

        Set_Render_Target(renderer, texture);
        SDLErrorHandle(SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255));
        SDLErrorHandle(SDL_RenderClear(renderer));

        SDL_Rect mouseRect = {
                (int) logical_mouse_x,
                (int) logical_mouse_y,
                3,
                3
        }; // This helps us ensure that the logical mouse position will remain where the mouse actually is visually


        SDLErrorHandle(SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255));
        SDLErrorHandle(SDL_RenderFillRect(renderer, &mouseRect));


        // render tilemap walls
        for(int row = 0; row < auto_tile_map.n_rows; row++)
        {
            for(int col = 0; col < auto_tile_map.n_cols; col++)
            {
                bool is_wall = AutoTiledTileMap_get_wall_value(auto_tile_map, row, col);
                if(is_wall)
                {
                    SDL_FRect wall_rect = {
                            tilemap_position.x + (float) (col * auto_tile_map.tile_size),
                            tilemap_position.y + (float) (row * auto_tile_map.tile_size),
                            (float) auto_tile_map.tile_size,
                            (float) auto_tile_map.tile_size
                    };

                    SDLErrorHandle(SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255));
                    SDLErrorHandle(SDL_RenderFillRectF(renderer, &wall_rect));
                }
            }
        }

        SDLErrorHandle(SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255));

        util_draw_grid(
                renderer,
                auto_tile_map.tile_size,
                tilemap_position.x,
                tilemap_position.y,
                auto_tile_map.n_rows,
                auto_tile_map.n_cols
        );

        //SDLErrorHandle(SDL_RenderSetScale(renderer, 1, 1)); // Set render scale back to 1, 1 before running imgui rendering, because imgui should stay the same size regardless of scale of the game world

        Set_Render_Target(renderer, NULL);



        //Render to screen
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);

        delta_time_frame_end_ticks = SDL_GetTicks();
    }

}