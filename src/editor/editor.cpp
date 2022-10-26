#include <filesystem>
#include <external/SDL2/include/SDL.h>
#include <src/tile/tilemap.h>
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


#define STB_IMAGE_IMPLEMENTATION

#include "stb/stb_image.h"



//---------------------------------------------------
ImVec2 dock_size{static_cast<float>(Global::window_w), static_cast<float>(Global::window_h)};
SDL_Texture *target_texture;
SDL_Texture *tileset_window;
SDL_Texture *tileset_texture;
SDL_Color background_color = {0x6E,0x62,0x59,0xFF};

enum EditMode
{
    EditMode_AutoTilePlacement,
    EditMode_StaticPelletPlacement
};

static EditMode edit_mode = EditMode_AutoTilePlacement;

WorldPosition tilemap_position = {0.0, 0.0};

int tilemap_mouse_row = 0;
int tilemap_mouse_col = 0;
int imgui_window_mouse_x = 0;
int imgui_window_mouse_y = 0;
int tileset_width = 1;
int tileset_height = 1;
int tileset_channels = 0;
float logical_mouse_x = 0;
float logical_mouse_y = 0;
float relative_tilemap_mouse_x;
float relative_tilemap_mouse_y;
v2d panning_offset = {0, 0};
v2d imgui_tilemap_position = {tilemap_position.x, tilemap_position.y};
v2d imgui_axis_position = {0, 0};
v2d imgui_offset_tilemap = {0, 0};
v2d imgui_offset_axis = {0, 0};
v2d imgui_window_global_offset = {0, 0};


ImVec2 autotiler_window_size_previous_frame;
ImVec2 autotiler_window_size_current_frame;
ImVec2 tileset_window_size_previous_frame;
ImVec2 tileset_window_size_current_frame;
ImVec2 imgui_window_pos;
ImVec2 window_center_popup{(Global::window_w / 2.0f) - 150.0f, (Global::window_h / 2.0f) - 100.0f};
ImVec2 window_size_popup{300, 85};
bool layout_initialized = false;


Uint32 imgui_tilemap_n_rows;
Uint32 imgui_tilemap_n_cols;
Uint32 imgui_save_notification_duration_ms = 3000;
int imgui_save_notification_timer = 0;
Uint32 imgui_tileset_n_rows = 8;
Uint32 imgui_tileset_n_cols = 20;
Uint32 tile_cell_size;
Uint32 mouse_button_state_current;
Uint32 mouse_button_state_prev;
char *imgui_tilemap_save_notification_text;
char *imgui_tilemap_save_notification_text_success = (char *) "Saved Successfully!";
char *imgui_tilemap_save_notification_text_failure = (char *) "Save Failed!!!!";

std::filesystem::path assets_rel_path_prefix = "assets/";
std::string input_text_image_file_path = "";
std::string full_image_file_path;

SDL_Color axis_color{0x00, 0xFF, 0x00, 0xFF};

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

        tileset_window_size_current_frame = {(float) (tilemap.tile_size * tilemap.n_cols),
                                             (float) (tilemap.tile_size * tilemap.n_rows)};
        tileset_window_size_previous_frame = tileset_window_size_current_frame;

        imgui_tilemap_n_rows = tilemap.n_rows;
        imgui_tilemap_n_cols = tilemap.n_cols;

        target_texture = Util::Texture_Create_Blank((int) autotiler_window_size_current_frame.x,
                                                    (int) autotiler_window_size_current_frame.y);

        tileset_window = Util::Texture_Create_Blank(tileset_width, tileset_height);
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


    //--------------------------------------------------------
    // this function should probably get refactored out eventually cause we'll need to use the same logic for all sprite sheets
    //
    // returns true if success, false if image_data is NULL
    bool
    TilesetLoad()
    {
        int req_format = STBI_rgb_alpha;
        full_image_file_path = assets_rel_path_prefix.string() + input_text_image_file_path;
        unsigned char *tileset_image_data = stbi_load(full_image_file_path.c_str(), &tileset_width, &tileset_height,
                                                      &tileset_channels, req_format);
        if (tileset_image_data == NULL)
        {
            return false;
        }

        tile_cell_size = tileset_width / imgui_tileset_n_cols;

        int depth, pitch;
        SDL_Surface *image_surface;
        Uint32 pixel_format;

        depth = 32;
        pitch = 4 * tileset_width;
        pixel_format = SDL_PIXELFORMAT_RGBA32;

        image_surface = SDL_CreateRGBSurfaceWithFormatFrom(tileset_image_data, tileset_width, tileset_height, depth,
                                                           pitch, pixel_format);
        SDLErrorHandleNull(image_surface);
        tileset_texture = SDL_CreateTextureFromSurface(Global::renderer, image_surface);
        SDLErrorHandleNull(tileset_texture);
        SDL_FreeSurface(image_surface);

        stbi_image_free(tileset_image_data);

        return true;


    }

    //--------------------------------------------------------
    void
    LoadTilesetImageResultPopupWindow()
    {
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

    //-----------------------------------------------------------
    void
    TilemapAndTilesetPropertiesPanelWindow(Tilemap &tilemap, TileBoundGoodPelletsPool &pellets_pool)
    {

        if (ImGui::Begin("Auto Tilemap Properties"))
        {



            ImGui::Text("Tile Map Properties");

            ImGui::Text("Edit Mode");
            ImGui::RadioButton("Auto Tiling Marker Placement", reinterpret_cast<int *>(&edit_mode), EditMode_AutoTilePlacement); ImGui::SameLine();
            ImGui::RadioButton("Static Pellet Placement", reinterpret_cast<int *>(&edit_mode), EditMode_StaticPelletPlacement);

            ImGui::DragFloat2("Offset Position", (float *) &tilemap_position);
            ImGui::InputInt("Tile Size", (int *) &tilemap.tile_size);
            ImGui::InputInt("Num Rows", (int *) &imgui_tilemap_n_rows);
            ImGui::InputInt("Num Cols", (int *) &imgui_tilemap_n_cols);
            if (ImGui::Button("Update Row/Col Dimensions"))
            {// Use a button so user can confirm this because it could lose data if subtracting rows/columns
                Tilemap_resize_and_shift_values(
                        &tilemap,
                        imgui_tilemap_n_rows,
                        imgui_tilemap_n_cols);
            }
            if (ImGui::Button("Reset New Row/Col to Current"))
            {// If user wants to reset to the current rows/cols because they realized they don't want to change it anymore
                imgui_tilemap_n_rows = tilemap.n_rows;
                imgui_tilemap_n_cols = tilemap.n_cols;
            }
            ImGui::Text("Window Mouse Pos:  (%d, %d)", imgui_window_mouse_x, imgui_window_mouse_y);
            ImGui::Text("Logical Mouse Pos: (%.2f, %.2f)", logical_mouse_x, logical_mouse_y);
            ImGui::Text("TileMap Index:     (%d, %d)", tilemap_mouse_row, tilemap_mouse_col);

            if (ImGui::Button("Save TileMap"))
            {
                bool tilemap_save_success = Tilemap_save_to_file("tilemap.json", tilemap);
                bool pellets_save_success = TileBoundGoodPelletsPoolSaveToFile("tileboundgoodpelletspool.json", pellets_pool);

                if (tilemap_save_success && pellets_save_success)
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

            ImGui::Separator();
            ImGui::Text("Tile Set Properties");

            ImGui::Text("%s", assets_rel_path_prefix.string().c_str());
            ImGui::SameLine();
            ImGui::InputText(" ", &input_text_image_file_path);
            ImGui::SameLine();
            
            if (ImGui::Button("Load Tileset"))
            {
                bool success = TilesetLoad();

                if (!success)
                {
                    ImGui::OpenPopup("TilesetLoadError");
                }
            }
            ImGui::Text("Texture Width: %d\tTexture Height:%d", tileset_width, tileset_height);
            
            ImGui::InputInt("Tileset Rows", (int *) &imgui_tileset_n_rows);
            ImGui::InputInt("Tileset Cols", (int *) &imgui_tileset_n_cols);
            ImGui::InputInt("Tileset Cell Size", (int *) &tile_cell_size);

        }
        LoadTilesetImageResultPopupWindow();
        ImGui::End();
        

    }


    //--------------------------------------------------------
    void
    TilemapAutoTilerWindow(Tilemap &tilemap, TileBoundGoodPelletsPool &pellets_pool)
    {
        mouse_button_state_prev = mouse_button_state_current;
        mouse_button_state_current = SDL_GetMouseState(&imgui_window_mouse_x, &imgui_window_mouse_y);
        

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});

        if (ImGui::Begin("Tilemap"))
        {
            if ((mouse_button_state_current & SDL_BUTTON_MMASK) && ImGui::IsWindowHovered())
            {
                ImGui::SetWindowFocus("Tilemap");
            }

            autotiler_window_size_previous_frame = autotiler_window_size_current_frame;
            autotiler_window_size_current_frame = ImGui::GetContentRegionAvail();

            if (autotiler_window_size_previous_frame.x != autotiler_window_size_current_frame.x ||
                autotiler_window_size_previous_frame.y != autotiler_window_size_current_frame.y)
            {
                SDL_DestroyTexture(target_texture);
                target_texture = Util::Texture_Create_Blank((int) autotiler_window_size_current_frame.x,
                                                            (int) autotiler_window_size_current_frame.y);
            }

            ImGui::Image(target_texture, autotiler_window_size_previous_frame);

            imgui_window_pos = ImGui::GetCursorScreenPos();

            // We calculate the WORLD(imgui window) to SCREEN position using the following formula.
            // nScreenX = fWorldX - offSetX
            // nScreenX = fWorldY - offSetY
            // Where in our case our world to screen should be 1 to 1. i.e. the world is being rendered at (fWorldX = 0, fWorldY = 0) relative to its display
            // along with our screen starting from (0,0), hence the reason fWorldX and fWorldY do not appear below.
            imgui_window_mouse_x -= (int) imgui_window_pos.x;

            // Since imgui has the convention of its screen position being measure from the BOTTOM LEFT CORNER rather than the TOP LEFT CORNER as one would usually be used to
            // we need to add the panel size to the screen position to get the offset we would normally use for our formula
            // i.e. we are transforming the y coodinate from bottomleft -> topleft.
            imgui_window_mouse_y -= ((int) imgui_window_pos.y - (int) autotiler_window_size_previous_frame.y);


            SDL_RenderWindowToLogical(
                    Global::renderer,
                    imgui_window_mouse_x,
                    imgui_window_mouse_y,
                    &logical_mouse_x,
                    &logical_mouse_y);


            // find out what tile the mouse cursor is in because this info is useful to the user
            relative_tilemap_mouse_x = logical_mouse_x - imgui_tilemap_position.x;
            relative_tilemap_mouse_y = logical_mouse_y - imgui_tilemap_position.y;

            tilemap_mouse_row = static_cast<int>(relative_tilemap_mouse_y / (float) tilemap.tile_size);
            tilemap_mouse_col = static_cast<int>(relative_tilemap_mouse_x / (float) tilemap.tile_size);

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
                            TilBoundGoodPelletsPoolCreatePellet(pellets_pool, tilemap,
                                                                (TileIndex) {tilemap_mouse_row, tilemap_mouse_col});
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
                            TilBoundGoodPelletsPoolRemovePellet(pellets_pool, tilemap,
                                                                (TileIndex) {tilemap_mouse_row, tilemap_mouse_col});
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


            Util::RenderTargetSet(Global::renderer, target_texture);
            SDLErrorHandle(SDL_SetRenderDrawColor(Global::renderer, 50, 50, 50, 255));
            SDLErrorHandle(SDL_RenderClear(Global::renderer));

            SDL_Rect mouseRect = {
                    (int) logical_mouse_x,
                    (int) logical_mouse_y,
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
                                    axis_color);

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
    void RenderTileset(int x, int y)
    {
        SDL_Rect renderQuad = {x, y, tileset_width, tileset_height};

        SDL_RenderCopy(Global::renderer, tileset_texture, NULL, &renderQuad);
    }


    //--------------------------------------------------------
    void
    TilesetBitmaskerWindow()
    {
        if (ImGui::Begin("Tileset"))
        {

            tileset_window_size_current_frame = {(float) tileset_width, (float) tileset_height};
            if (tileset_window_size_previous_frame.x != tileset_window_size_current_frame.x ||
                tileset_window_size_previous_frame.y != tileset_window_size_current_frame.y)
            {
                SDL_DestroyTexture(tileset_window);
                tileset_window = Util::Texture_Create_Blank((int) tileset_window_size_current_frame.x,
                                                            (int) tileset_window_size_current_frame.y);
            }

            ImGui::Image(tileset_window, tileset_window_size_current_frame);
            Util::RenderTargetSet(Global::renderer, tileset_window);
            SDLErrorHandle(SDL_SetRenderDrawColor(Global::renderer, background_color.r, background_color.g, background_color.b, background_color.a));
            SDLErrorHandle(SDL_RenderClear(Global::renderer));
            RenderTileset(0, 0);
            Util::DrawGrid(Global::renderer, tile_cell_size, 0, 0, imgui_tileset_n_rows, imgui_tileset_n_cols,
                           SDL_Color());

        }
        ImGui::End();
    }


    //--------------------------------------------------------
    void EditorWindow(Tilemap &tilemap, TileBoundGoodPelletsPool &pellets_pool)
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
        TilemapAutoTilerWindow(tilemap, pellets_pool);
        TilesetBitmaskerWindow();
        TilemapAndTilesetPropertiesPanelWindow(tilemap, pellets_pool);

        ImGui::PopStyleVar();
        ImGui::End();
    }
}
