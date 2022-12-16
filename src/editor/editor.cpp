#include <filesystem>
#include <external/SDL2/include/SDL.h>
#include <src/tile/tilemap.h>
#include <src/tile/tileset.h>
#include <external/imgui/imgui_internal.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <src/util/util_error_handling.h>
#include <src/util/util_load_save.h>
#include <src/util/util_draw.h>
#include <src/util/util_panning.h>
#include <src/global.h>
#include <string>
#include <src/util/util_misc.h>
#include <src/pellet_pools/tile_bound_good_pellets_pool.h>
#include <src/editor/level_data.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"




enum EditMode
{
    EditMode_AutoTilePlacement,
    EditMode_StaticPelletPlacement
};




// shared between tilemap window and tilemap properties window
WorldPosition tilemap_position = {0.0, 0.0};
int tilemap_mouse_row = 0;
int tilemap_mouse_col = 0;
float imgui_logical_mouse_x = 0;
float imgui_logical_mouse_y = 0;
int imgui_window_mouse_x = 0;
int imgui_window_mouse_y = 0;
static EditMode edit_mode = EditMode_AutoTilePlacement;


// shared between tileset bitmasker and tileset properties
int imgui_tileset_window_mouse_x = 0;
int imgui_tileset_window_mouse_y = 0;


// constants 
const char *SAVE_NOTIFICATION_TEXT_SUCCESS = (char *) "Saved Successfully!";
const char *SAVE_NOTIFICATION_TEXT_FAILURE = (char *) "Save Failed!!!!";
const SDL_Color LINE_COLOR{0x00, 0xFF, 0xFF, 0xFF};
const SDL_Color AXIS_COLOR{0x00, 0xFF, 0x00, 0xFF};
const SDL_Color BACKGROUND_COLOR = {0x6E, 0x62, 0x59, 0xFF};




namespace Editor
{


    //--------------------------------------------------------
    void
    DockingLayoutPreset(ImGuiID dockspace_id)
    {
        static ImVec2 dock_size{static_cast<float>(Global::window_w), static_cast<float>(Global::window_h)};

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, dock_size);
        ImGuiID right;
        ImGuiID left;

        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.3f, &right, &left);
        ImGui::DockBuilderDockWindow("Tilemap Properties", right);
        ImGui::DockBuilderDockWindow("Tileset Properties", right);
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
        static bool layout_initialized = false;

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
    LoadTilesetImageResultPopupWindow()
    {
        ImVec2 window_size_popup{300, 85};
        ImVec2 window_center_popup{(Global::window_w / 2.0f) - 150.0f, (Global::window_h / 2.0f) - 100.0f};


        if (ImGui::BeginPopupModal("TilesetLoadError"))
        {
            ImGui::SetWindowSize(window_size_popup);
            ImGui::SetWindowPos(window_center_popup);
            ImGui::Text("ERROR: Either FILE does not exist\nor FILEPATH is invalid!");

            ImGui::SetCursorPos(ImVec2{80, 50});
            if (ImGui::Button("Ok", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }


    void TilemapPropsPanelWindow(Tilemap& tilemap, TileBoundGoodPelletsPool& pellets_pool)
    {
        static Uint32 imgui_tilemap_n_rows = tilemap.n_rows;
        static Uint32 imgui_tilemap_n_cols = tilemap.n_cols;
        
        
        if (ImGui::Begin("Tilemap Properties"))
        {
            ImGui::Text("Tile Map Properties");

            ImGui::Text("Edit Mode");
            ImGui::RadioButton("Auto Tiling Marker Placement", reinterpret_cast<int *>(&edit_mode), EditMode_AutoTilePlacement);
            ImGui::SameLine();
            ImGui::RadioButton("Static Pellet Placement", reinterpret_cast<int *>(&edit_mode), EditMode_StaticPelletPlacement);

            ImGui::DragFloat2("Offset Position", (float *) &tilemap_position);
            ImGui::InputInt("Tile Size", (int *) &tilemap.tile_size);
            ImGui::InputInt("Num Rows", (int *) &imgui_tilemap_n_rows);
            ImGui::InputInt("Num Cols", (int *) &imgui_tilemap_n_cols);
            if (ImGui::Button("update row/col dimensions"))
            {// use a button so user can confirm this because it could lose data if subtracting rows/columns
                bool_vector_2d_resize_and_shift_values(tilemap.is_collision_tiles, tilemap.n_rows, tilemap.n_cols, imgui_tilemap_n_rows, imgui_tilemap_n_cols);
                bool_vector_2d_resize_and_shift_values(pellets_pool.is_active, tilemap.n_rows, tilemap.n_cols, imgui_tilemap_n_rows, imgui_tilemap_n_cols);
                tilemap.n_rows = imgui_tilemap_n_rows;
                tilemap.n_cols = imgui_tilemap_n_cols;
            }
            if (ImGui::Button("reset new row/col to current"))
            {// if user wants to reset to the current rows/cols because they realized they don't want to change it anymore
                imgui_tilemap_n_rows = tilemap.n_rows;
                imgui_tilemap_n_cols = tilemap.n_cols;
            }
            ImGui::Text("Window Mouse Pos:  (%d, %d)", imgui_window_mouse_x, imgui_window_mouse_y);
            ImGui::Text("Logical Mouse Pos: (%.2f, %.2f)", imgui_logical_mouse_x, imgui_logical_mouse_y);
            ImGui::Text("TileMap Index:     (%d, %d)", tilemap_mouse_row, tilemap_mouse_col);

            if (ImGui::Button("Save TileMap"))
            {
                bool tilemap_save_success = TilemapSaveToFile("tilemap.json", tilemap);
                bool pellets_save_success = TileBoundGoodPelletsPoolSaveToFile("tileboundgoodpelletspool.json", pellets_pool);
                char *save_notification_text;

                if (tilemap_save_success && pellets_save_success)
                {
                    save_notification_text = (char *)SAVE_NOTIFICATION_TEXT_SUCCESS;
                }
                else
                {
                    save_notification_text = (char *)SAVE_NOTIFICATION_TEXT_FAILURE;
                }
                time_t save_timestamp = time(NULL);
                printf("%s, %s", save_notification_text, ctime(&save_timestamp));
                save_timestamp = time(NULL);
            }
        }
        ImGui::End();
    }


    void TilesetPropsPanelWindow(Tileset &tileset)
    {
        const std::filesystem::path assets_rel_path_prefix = "assets/";
        static std::string input_text_image_file_path = "";
        static std::string full_image_file_path = "";
        static Uint32 input_tileset_n_rows = tileset.sprite_sheet.n_rows;
        static Uint32 input_tileset_n_cols = tileset.sprite_sheet.n_cols;

        if (ImGui::Begin("Tileset Properties"))
        {

            ImGui::Text("Tile Set Properties");

            ImGui::Text("%s", assets_rel_path_prefix.string().c_str());
            ImGui::SameLine();
            ImGui::InputText(" ", &input_text_image_file_path);
            ImGui::SameLine();


            if (ImGui::Button("Load Tileset"))
            {
                bool success = TilesetCreateNewTextureandTileset(Global::renderer, tileset, assets_rel_path_prefix.string() + input_text_image_file_path, input_tileset_n_rows, input_tileset_n_cols);

                if (!success)
                {
                    ImGui::OpenPopup("TilesetLoadError");
                }
            }

            ImGui::Text("Texture Width: %f\tTexture Height:%f", tileset.sprite_sheet.texture_w, tileset.sprite_sheet.texture_h);
            ImGui::Text("Tileset Mouse Pos:  (%d, %d)", imgui_tileset_window_mouse_x, imgui_tileset_window_mouse_y);
            ImGui::Text("Tileset Tile Width: %f\t Tile Height: %f)", tileset.sprite_sheet.cell_width(), tileset.sprite_sheet.cell_height());
            ImGui::InputInt("Tileset Rows", (int *) &input_tileset_n_rows);
            ImGui::InputInt("Tileset Cols", (int *) &input_tileset_n_cols);
            if (ImGui::Button("Resize Tileset Row/Col Dimensions"))
            {
                TilesetResizeandShiftValues(
                        tileset,
                        input_tileset_n_rows,
                        input_tileset_n_cols);
            }


            if (ImGui::Button("Save Tileset"))
            {
                bool success = TilesetSaveToFile("assets/tileset.json", tileset);
                char *imgui_tileset_save_notification_text;
                if (success)
                {
                    imgui_tileset_save_notification_text = (char *)SAVE_NOTIFICATION_TEXT_SUCCESS;
                }
                else
                {
                    imgui_tileset_save_notification_text = (char *)SAVE_NOTIFICATION_TEXT_FAILURE;
                }
                time_t save_timestamp = time(NULL);
                printf("%s, %s", imgui_tileset_save_notification_text, ctime(&save_timestamp));
                save_timestamp = time(NULL);
            }

        }
        ImGui::End();
    }


    //-----------------------------------------------------------
    void
    TilemapAndTilesetPropertiesPanelWindow(LevelData & level_data)
    {
        TilemapPropsPanelWindow(level_data.tilemap, level_data.pellets_pool);
        TilesetPropsPanelWindow(level_data.tileset);
        LoadTilesetImageResultPopupWindow();
    }


    //--------------------------------------------------------
    void
    TilemapAutoTilerWindow(Tilemap &tilemap, TileBoundGoodPelletsPool &pellets_pool)
    {
        static v2d imgui_tilemap_position = {tilemap_position.x, tilemap_position.y};

        static v2d imgui_window_global_offset = {0, 0};
        static v2d panning_offset = {0, 0};
        static v2d imgui_axis_position = {0, 0};
        static v2d imgui_offset_tilemap = {0, 0};
        static v2d imgui_offset_axis = {0, 0};
        static ImVec2 autotiler_window_size_previous_frame = {(float) (tilemap.tile_size * tilemap.n_cols),
                                                           (float) (tilemap.tile_size * tilemap.n_rows)};
        static ImVec2 autotiler_window_size_current_frame = autotiler_window_size_previous_frame;
        static SDL_Texture *tilemap_target_texture = Util::Texture_Create_Blank(
                                                    (int) tilemap.tile_size * tilemap.n_cols,
                                                    (int) tilemap.tile_size * tilemap.n_rows);
        static int window_mouse_x = 0 , window_mouse_y = 0;
        static Uint32 mouse_button_state_current = SDL_GetMouseState(&window_mouse_x, &window_mouse_y);
        static Uint32 mouse_button_state_prev = mouse_button_state_current;
        mouse_button_state_prev = mouse_button_state_current;
        mouse_button_state_current = SDL_GetMouseState(&window_mouse_x, &window_mouse_y);\

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});

        if (ImGui::Begin("Tilemap"))
        {
            /*if ((mouse_button_state_current & SDL_BUTTON_MMASK) && ImGui::IsWindowHovered())
            {
                ImGui::SetWindowFocus("Tilemap");
            }*/

            autotiler_window_size_previous_frame = autotiler_window_size_current_frame;
            autotiler_window_size_current_frame = ImGui::GetContentRegionAvail();

            if (autotiler_window_size_previous_frame.x != autotiler_window_size_current_frame.x ||
                autotiler_window_size_previous_frame.y != autotiler_window_size_current_frame.y)
            {
                SDL_DestroyTexture(tilemap_target_texture);
                tilemap_target_texture = Util::Texture_Create_Blank((int) autotiler_window_size_current_frame.x,
                                                            (int) autotiler_window_size_current_frame.y);
            }

            ImGui::Image(tilemap_target_texture, autotiler_window_size_previous_frame);

            ImVec2 imgui_window_pos = ImGui::GetCursorScreenPos();

            // We calculate the WORLD(imgui window) to SCREEN position using the following formula.
            // nScreenX = fWorldX - offSetX
            // nScreenX = fWorldY - offSetY
            // Where in our case our world to screen should be 1 to 1. i.e. the world is being rendered at (fWorldX = 0, fWorldY = 0) relative to its display
            // along with our screen starting from (0,0), hence the reason fWorldX and fWorldY do not appear below.
            imgui_window_mouse_x = window_mouse_x - (int) imgui_window_pos.x;

            // Since imgui has the convention of its screen position being measure from the BOTTOM LEFT CORNER rather than the TOP LEFT CORNER as one would usually be used to
            // we need to add the panel size to the screen position to get the offset we would normally use for our formula
            // i.e. we are transforming the y coodinate from bottomleft -> topleft.
            imgui_window_mouse_y = window_mouse_y - ((int) imgui_window_pos.y - (int) autotiler_window_size_previous_frame.y);


            SDL_RenderWindowToLogical(
                    Global::renderer,
                    imgui_window_mouse_x,
                    imgui_window_mouse_y,
                    &imgui_logical_mouse_x,
                    &imgui_logical_mouse_y);


            // find out what tile the mouse cursor is in because this info is useful to the user
            int relative_tilemap_mouse_x = imgui_logical_mouse_x - imgui_tilemap_position.x;
            int relative_tilemap_mouse_y = imgui_logical_mouse_y - imgui_tilemap_position.y;
            if (relative_tilemap_mouse_x >= 0 && relative_tilemap_mouse_y >= 0)
            {
                tilemap_mouse_row = static_cast<int>(relative_tilemap_mouse_y / (float) tilemap.tile_size);
                tilemap_mouse_col = static_cast<int>(relative_tilemap_mouse_x / (float) tilemap.tile_size);
            }

            if ((mouse_button_state_current & SDL_BUTTON_MMASK) && ImGui::IsWindowHovered())
            {
                ImGui::SetWindowFocus("Tilemap");
            }


            if(ImGui::IsWindowFocused())
            {
                if(tilemap_mouse_row >= 0 && tilemap_mouse_col >= 0
                   && tilemap_mouse_row < tilemap.n_rows && tilemap_mouse_col < tilemap.n_cols)
                {
                    if (mouse_button_state_current & SDL_BUTTON_LMASK)
                    {

                        if(edit_mode == EditMode_AutoTilePlacement)
                        {
                            Tilemap_set_collision_tile_value(&tilemap, tilemap_mouse_row, tilemap_mouse_col, true);
                        }
                        else if(edit_mode == EditMode_StaticPelletPlacement)
                        {
                            TileIndex tile_index = {tilemap_mouse_row, tilemap_mouse_col};
                            TilBoundGoodPelletsPoolCreatePellet(pellets_pool, tilemap, tile_index);
                        }


                    }

                    else if (mouse_button_state_current & SDL_BUTTON_RMASK)
                    {

                        if(edit_mode == EditMode_AutoTilePlacement)
                        {
                            Tilemap_set_collision_tile_value(&tilemap, tilemap_mouse_row, tilemap_mouse_col, false);
                        }
                        else if(edit_mode == EditMode_StaticPelletPlacement)
                        {
                            TileIndex tile_index = {tilemap_mouse_row, tilemap_mouse_col};
                            TilBoundGoodPelletsPoolRemovePellet(pellets_pool, tilemap, tile_index);
                        }

                    }
                }

                if ((mouse_button_state_prev & SDL_BUTTON_MMASK))
                {
                    imgui_window_global_offset.x -= (float) (imgui_window_mouse_x - panning_offset.x);
                    imgui_window_global_offset.y -= (float) (imgui_window_mouse_y - panning_offset.y);

                    panning_offset.x = imgui_window_mouse_x;
                    panning_offset.y = imgui_window_mouse_y;
                }
                else if ((mouse_button_state_current & SDL_BUTTON_MMASK) )
                {
                    panning_offset.x = imgui_window_mouse_x;
                    panning_offset.y = imgui_window_mouse_y;
                }

            }


            Util::RenderTargetSet(Global::renderer, tilemap_target_texture);
            SDLErrorHandle(SDL_SetRenderDrawColor(Global::renderer, 50, 50, 50, 255));
            SDLErrorHandle(SDL_RenderClear(Global::renderer));

            SDL_Rect mouseRect = {
                    (int) imgui_logical_mouse_x,
                    (int) imgui_logical_mouse_y,
                    3,
                    3};// This helps us ensure that the logical mouse position will remain where the mouse actually is visually


            SDLErrorHandle(SDL_SetRenderDrawColor(Global::renderer, 0, 255, 255, 255));
            SDLErrorHandle(SDL_RenderFillRect(Global::renderer, &mouseRect));

            imgui_offset_tilemap.x = tilemap_position.x;
            imgui_offset_tilemap.y = tilemap_position.y;

            imgui_tilemap_position = Util::ImGuiOffsetCoordinatesToImGuiScreenCoordinates(imgui_window_global_offset.x,
                                                      imgui_window_global_offset.y,
                                                      imgui_offset_tilemap.x,
                                                      imgui_offset_tilemap.y);

            imgui_axis_position = Util::ImGuiOffsetCoordinatesToImGuiScreenCoordinates(imgui_window_global_offset.x,
                                                      imgui_window_global_offset.y,
                                                      imgui_offset_axis.x,
                                                      imgui_offset_axis.y);

            Util::RenderInfiniteAxis(Global::renderer, 
                                    autotiler_window_size_current_frame.y,
                                    autotiler_window_size_current_frame.x,
                                    imgui_axis_position.x, imgui_axis_position.y,
                                    AXIS_COLOR);

            // render tilemap collision_tiles
            TilemapCollisionTileRectsRender(tilemap,
                                            imgui_tilemap_position.x,
                                            imgui_tilemap_position.y,
                                            SDL_Color{255, 0, 0, 255});
            TilemapGridRender(tilemap,
                              imgui_tilemap_position.x,
                              imgui_tilemap_position.y,
                              SDL_Color{100, 100, 100, 255});
            TileBoundGoodPelletsPoolRender(pellets_pool, tilemap, imgui_tilemap_position);
        }
        ImGui::End();
    }




    //--------------------------------------------------------
    void
    TilesetBitmaskerWindow(Tileset &tileset)
    {
        static SDL_Texture *tileset_target_texture = Util::Texture_Create_Blank(tileset.sprite_sheet.texture_w, tileset.sprite_sheet.texture_h);

        
        static ImVec2 tileset_window_size_current_frame = {(float) (tileset.sprite_sheet.cell_width() * tileset.sprite_sheet.n_cols),
                                                           (float) (tileset.sprite_sheet.cell_height() * tileset.sprite_sheet.n_rows)};
        static ImVec2 tileset_window_size_previous_frame = tileset_window_size_current_frame;
        static Uint32 mouse_button_state_current = SDL_GetMouseState(&imgui_tileset_window_mouse_x, &imgui_tileset_window_mouse_y);
        if (ImGui::Begin("Tileset"))
        {

            tileset_window_size_current_frame = {(float) tileset.sprite_sheet.texture_w, (float) tileset.sprite_sheet.texture_h};
            if (tileset_window_size_previous_frame.x != tileset_window_size_current_frame.x ||
                tileset_window_size_previous_frame.y != tileset_window_size_current_frame.y)
            {
                SDL_DestroyTexture(tileset_target_texture);
                tileset_target_texture = Util::Texture_Create_Blank((int) tileset_window_size_current_frame.x,
                                                            (int) tileset_window_size_current_frame.y);
            }https://discord.com/snowsgiving

            ImGui::Image(tileset_target_texture, tileset_window_size_current_frame);

            
            ImVec2 imgui_tileset_window_pos = ImGui::GetCursorScreenPos();
            imgui_tileset_window_mouse_x -= (int) imgui_tileset_window_pos.x;
            imgui_tileset_window_mouse_y -= ((int) imgui_tileset_window_pos.y - (int) tileset_window_size_current_frame.y);


            if (mouse_button_state_current & SDL_BUTTON_LMASK && ImGui::IsWindowFocused())
            {
                TilesetSetBitMaskTile(tileset, imgui_tileset_window_mouse_x, imgui_tileset_window_mouse_y);
            }
            else if (mouse_button_state_current & SDL_BUTTON_RMASK && ImGui::IsWindowFocused())
            {
                TilesetUnsetBitMaskTile(tileset, imgui_tileset_window_mouse_x, imgui_tileset_window_mouse_y);
            }


            Util::RenderTargetSet(Global::renderer, tileset_target_texture);


            SDLErrorHandle(SDL_SetRenderDrawColor(Global::renderer, BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, BACKGROUND_COLOR.a));
            SDLErrorHandle(SDL_RenderClear(Global::renderer));

            RenderTileset(Global::renderer, tileset, 0, 0);
            BitmaskRender(Global::renderer, tileset);

            Util::DrawGrid(Global::renderer, tileset.sprite_sheet.cell_width(), tileset.sprite_sheet.cell_height(), 0, 0, tileset.sprite_sheet.n_rows, tileset.sprite_sheet.n_cols,
                           SDL_Color{100, 100, 100, 255});
        }
        ImGui::End();
        LoadTilesetImageResultPopupWindow();
    }



    //--------------------------------------------------------
    void EditorWindow(LevelData & level_data)
    {
        static bool is_dock_space_open = true;

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

        ImGui::Begin("Editor", &is_dock_space_open, window_flags);
        ImGui::PopStyleVar(3);


        DockSpaceSetup();
        TilemapAutoTilerWindow(level_data.tilemap, level_data.pellets_pool);
        TilesetBitmaskerWindow(level_data.tileset);
        TilemapAndTilesetPropertiesPanelWindow(level_data);

        ImGui::PopStyleVar();
        ImGui::End();
    }
}
