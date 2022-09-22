//
// Created by frog on 9/19/22.
//
#include "SDL2/include/SDL.h"
#include "src/engine/tile/tilemap.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui.h"
#include "src/engine/util/util_error_handling.h"
#include "src/engine/util/util_load_save.h"
#include "src/engine/util/util_draw.h"
#include "src/engine/global.h"


ImVec2 dock_size{static_cast<float>(Global::window_w), static_cast<float>(Global::window_h)};
SDL_Texture *target_texture;


WorldPosition tilemap_position = {0.0, 0.0};

int tilemap_row_mouse_on_display = 0;
int tilemap_col_mouse_on_display = 0;
int window_mouse_x = 0;
int window_mouse_y = 0;
float logical_mouse_x = 0;
float logical_mouse_y = 0;
float relative_tilemap_mouse_x;
float relative_tilemap_mouse_y;

ImVec2 autotiler_window_size_previous_frame;
ImVec2 autotiler_window_size_current_frame;
ImVec2 screen_mouse_pos;
bool layout_initialized = false;


Uint32 imgui_tilemap_n_rows;
Uint32 imgui_tilemap_n_cols;
Uint32 imgui_save_notification_duration_ms = 3000;
Uint32 imgui_save_notification_timer = 0;
char *imgui_tilemap_save_notification_text;
char *imgui_tilemap_save_notification_text_success = (char *) "Saved Successfully!";
char *imgui_tilemap_save_notification_text_failure = (char *) "Save Failed!!!!";


static bool dockSpaceOpen = true;

namespace Editor
{
    //----------------------------------------------------------
    void
    Setup(Tilemap &tilemap)
    {
        autotiler_window_size_current_frame = {(float) (tilemap.tile_size * tilemap.n_cols),
                                               (float) (tilemap.tile_size * tilemap.n_rows)};
        autotiler_window_size_previous_frame = autotiler_window_size_current_frame;

        imgui_tilemap_n_rows = tilemap.n_rows;
        imgui_tilemap_n_cols = tilemap.n_cols;

        target_texture = Util::Texture_Create_Blank((int) autotiler_window_size_current_frame.x,
                                                    (int) autotiler_window_size_current_frame.y);
    }


    //--------------------------------------------------------
    void
    DockingLayoutPreset(ImGuiID dockspace_id)
    {
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, dock_size);
        ImGuiID right;
        ImGuiID left;

        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.3f, &right, &left);
        ImGui::DockBuilderDockWindow("Auto Tilemap Properties", right);
        ImGui::DockBuilderDockWindow("Tilemap", left);
        ImGui::DockBuilderDockWindow("Tileset", left);

        // Uncomment this part out if you want to keep the tilemap window fixed in its docked position
        //ImGuiDockNode* node = ImGui::DockBuilderGetNode(left);
        //node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

        ImGui::DockBuilderFinish(dockspace_id);
    }


    //----------------------------------------------------------
    void
    DockSpaceSetup()
    {
        ImGuiID dockspace_id;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            dockspace_id = ImGui::GetID("EditorDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        if (!layout_initialized)
        {

            DockingLayoutPreset(dockspace_id);
            layout_initialized = true;
        }
    }


    //--------------------------------------------------------
    void
    MenuBar()
    {
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
    }


    //-----------------------------------------------------------
    void
    TilemapPropertiesPanelWindow(Tilemap &tilemap)
    {
        if (ImGui::Begin("Auto Tilemap Properties"))
        {

            ImGui::Text("Tile Map Properties");
            ImGui::DragFloat2("Offset Position", (float *) &tilemap_position);
            ImGui::InputInt("Tile Size", (int *) &tilemap.tile_size);
            ImGui::InputInt("Num Rows", (int *) &imgui_tilemap_n_rows);
            ImGui::InputInt("Num Cols", (int *) &imgui_tilemap_n_cols);
            if (ImGui::Button("Update Row/Col Dimensions"))
            { // Use a button so user can confirm this because it could lose data if subtracting rows/columns
                Tilemap_resize_and_shift_values(
                        &tilemap,
                        imgui_tilemap_n_rows,
                        imgui_tilemap_n_cols
                );
            }
            if (ImGui::Button("Reset New Row/Col to Current"))
            { // If user wants to reset to the current rows/cols because they realized they don't want to change it anymore
                imgui_tilemap_n_rows = tilemap.n_rows;
                imgui_tilemap_n_cols = tilemap.n_cols;
            }
            ImGui::Text("Window Mouse Pos:  (%d, %d)", window_mouse_x, window_mouse_y);
            ImGui::Text("Logical Mouse Pos: (%.2f, %.2f)", logical_mouse_x, logical_mouse_y);
            ImGui::Text("TileMap Index:     (%d, %d)", tilemap_row_mouse_on_display, tilemap_col_mouse_on_display);

            if (ImGui::Button("Save TileMap"))
            {
                bool success = Tilemap_save_to_file("tilemap.json", tilemap);
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
                imgui_save_notification_timer -= Global::delta_time_ms;
                ImGui::Text("%s\n", imgui_tilemap_save_notification_text);
            }
        }
        ImGui::End();
    }


    //--------------------------------------------------------
    void AutoTilerWindow(Tilemap &tilemap)
    {
        Uint32 mouse_button_state = SDL_GetMouseState(&window_mouse_x, &window_mouse_y);

        // We calculate the WORLD(imgui window) to SCREEN position using the following formula.
        // nScreenX = fWorldX - offSetX
        // nScreenX = fWorldY - offSetY
        // Where in our case our world to screen should be 1 to 1. i.e. the world is being rendered at (fWorldX = 0, fWorldY = 0) relative to its display
        // along with our screen starting from (0,0), hence the reason fWorldX and fWorldY do not appear below.
        window_mouse_x -= (int) screen_mouse_pos.x;

        // Since imgui has the convention of its screen position being measure from the BOTTOM LEFT CORNER rather than the TOP LEFT CORNER as one would usually be used to
        // we need to add the panel size to the screen position to get the offset we would normally use for our formula
        // i.e. we are transforming the y coodinate from bottomleft -> topleft.
        window_mouse_y -= ((int) screen_mouse_pos.y - (int) autotiler_window_size_previous_frame.y);

        SDL_RenderWindowToLogical(
                Global::renderer,
                window_mouse_x,
                window_mouse_y,
                &logical_mouse_x,
                &logical_mouse_y
        );

        // find out what tile the mouse cursor is in because this info is useful to the user
        relative_tilemap_mouse_x = logical_mouse_x - tilemap_position.x;
        relative_tilemap_mouse_y = logical_mouse_y - tilemap_position.y;
        if (relative_tilemap_mouse_x >= 0 && relative_tilemap_mouse_y >= 0)
        {
            tilemap_row_mouse_on_display = static_cast<int>(relative_tilemap_mouse_y / (float) tilemap.tile_size);
            tilemap_col_mouse_on_display = static_cast<int>(relative_tilemap_mouse_x / (float) tilemap.tile_size);
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});

        if (ImGui::Begin("Tilemap"))
        {
            autotiler_window_size_previous_frame = autotiler_window_size_current_frame;
            autotiler_window_size_current_frame = ImGui::GetContentRegionAvail();

            if (autotiler_window_size_previous_frame.x != autotiler_window_size_current_frame.x
                || autotiler_window_size_previous_frame.y != autotiler_window_size_current_frame.y)
            {
                SDL_DestroyTexture(target_texture);
                target_texture = Util::Texture_Create_Blank((int) autotiler_window_size_current_frame.x,
                                                            (int) autotiler_window_size_current_frame.y);
            }

            ImGui::Image(target_texture, autotiler_window_size_previous_frame);
            screen_mouse_pos = ImGui::GetCursorScreenPos();

            if (mouse_button_state & SDL_BUTTON_LMASK && ImGui::IsWindowFocused())
            {
                bool tile_selected = false;
                int row_selected;
                int col_selected;

                relative_tilemap_mouse_x = logical_mouse_x - tilemap_position.x;
                relative_tilemap_mouse_y = logical_mouse_y - tilemap_position.y;

                if (relative_tilemap_mouse_x >= 0 && relative_tilemap_mouse_y >= 0)
                {
                    row_selected = static_cast<int>(round(relative_tilemap_mouse_y)) / tilemap.tile_size;
                    col_selected = static_cast<int>(round(relative_tilemap_mouse_x)) / tilemap.tile_size;

                    if (row_selected < tilemap.n_rows && col_selected < tilemap.n_cols)
                    {
                        tile_selected = true;
                    }
                }

                if (tile_selected)
                {
                    Tilemap_set_collision_tile_value(&tilemap, row_selected, col_selected, true);
                }
            }
            else if (mouse_button_state & SDL_BUTTON_RMASK && ImGui::IsWindowFocused())
            {
                bool tile_selected = false;
                int row_selected;
                int col_selected;

                relative_tilemap_mouse_x = logical_mouse_x - tilemap_position.x;
                relative_tilemap_mouse_y = logical_mouse_y - tilemap_position.y;

                if (relative_tilemap_mouse_x >= 0 && relative_tilemap_mouse_y >= 0)
                {
                    row_selected = static_cast<int>(relative_tilemap_mouse_y / (float) tilemap.tile_size);
                    col_selected = static_cast<int>(relative_tilemap_mouse_x / (float) tilemap.tile_size);

                    if (row_selected < tilemap.n_rows && col_selected < tilemap.n_cols)
                    {
                        tile_selected = true;
                    }
                }

                if (tile_selected)
                {
                    Tilemap_set_collision_tile_value(&tilemap, row_selected, col_selected, false);
                }

            }

            Util::RenderTargetSet(Global::renderer, target_texture);
            SDLErrorHandle(SDL_SetRenderDrawColor(Global::renderer, 50, 50, 50, 255));
            SDLErrorHandle(SDL_RenderClear(Global::renderer));

            SDL_Rect mouseRect = {
                    (int) logical_mouse_x,
                    (int) logical_mouse_y,
                    3,
                    3
            }; // This helps us ensure that the logical mouse position will remain where the mouse actually is visually


            SDLErrorHandle(SDL_SetRenderDrawColor(Global::renderer, 0, 255, 255, 255));
            SDLErrorHandle(SDL_RenderFillRect(Global::renderer, &mouseRect));


            // render tilemap collision_tiles
            for (int row = 0; row < tilemap.n_rows; row++)
            {
                for (int col = 0; col < tilemap.n_cols; col++)
                {
                    bool is_collidable_tile = (bool) stdvector_at_2d<char>(tilemap.is_collision_tiles, row, col,
                                                                           tilemap.n_cols);
                    if (is_collidable_tile)
                    {
                        SDL_FRect wall_rect = {
                                tilemap_position.x + (float) (col * tilemap.tile_size),
                                tilemap_position.y + (float) (row * tilemap.tile_size),
                                (float) tilemap.tile_size,
                                (float) tilemap.tile_size
                        };

                        SDLErrorHandle(SDL_SetRenderDrawColor(Global::renderer, 255, 0, 0, 255));
                        SDLErrorHandle(SDL_RenderFillRectF(Global::renderer, &wall_rect));
                    }
                }
            }

            SDLErrorHandle(SDL_SetRenderDrawColor(Global::renderer, 100, 100, 100, 255));

            Util::DrawGrid(
                    Global::renderer,
                    tilemap.tile_size,
                    tilemap_position.x,
                    tilemap_position.y,
                    tilemap.n_rows,
                    tilemap.n_cols
            );
        }
        ImGui::End();
    }


    //--------------------------------------------------------
    void
    TilesetBitmaskerWindow()
    {
        if (ImGui::Begin("Tileset"))
        {
            ImGui::Text("Test");

        }
        ImGui::End();
    }


    //--------------------------------------------------------
    void EditorWindow(Tilemap &tilemap)
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("Editor", &dockSpaceOpen, window_flags);
        ImGui::PopStyleVar(3);

        DockSpaceSetup();
        MenuBar();
        AutoTilerWindow(tilemap);
        TilesetBitmaskerWindow();
        TilemapPropertiesPanelWindow(tilemap);

        ImGui::PopStyleVar();
        ImGui::End();
    }
}
