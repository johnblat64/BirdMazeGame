#include <src/tile/tilemap.h>
#include <src/pellet_pools/tile_bound_good_pellets_pool.h>
#include <external/imgui/imgui_internal.h>
#include <external/imgui/imgui.h>

namespace PropertiesPanels
{

    void TilemapPropsPanelWindow(Tilemap &tilemap, TileBoundGoodPelletsPool &pellets_pool)
    {
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
            if (ImGui::Button("Update Row/Col Dimensions"))
            {// Use a button so user can confirm this because it could lose data if subtracting rows/columns
                bool_vector_2d_resize_and_shift_values(tilemap.is_collision_tiles, tilemap.n_rows, tilemap.n_cols, imgui_tilemap_n_rows, imgui_tilemap_n_cols);
                bool_vector_2d_resize_and_shift_values(pellets_pool.is_active, tilemap.n_rows, tilemap.n_cols, imgui_tilemap_n_rows, imgui_tilemap_n_cols);
                tilemap.n_rows = imgui_tilemap_n_rows;
                tilemap.n_cols = imgui_tilemap_n_cols;
            }
            if (ImGui::Button("Reset New Row/Col to Current"))
            {// If user wants to reset to the current rows/cols because they realized they don't want to change it anymore
                imgui_tilemap_n_rows = tilemap.n_rows;
                imgui_tilemap_n_cols = tilemap.n_cols;
            }
            ImGui::Text("Window Mouse Pos:  (%d, %d)", imgui_window_mouse_x, imgui_window_mouse_y);
            ImGui::Text("Logical Mouse Pos: (%.2f, %.2f)", logical_mouse_x, logical_mouse_y);
            ImGui::Text("TileMap Index:     (%d, %d)", tilemap_row_mouse_on_display, tilemap_col_mouse_on_display);

            if (ImGui::Button("Save TileMap"))
            {
                bool tilemap_save_success = TilemapSaveToFile("tilemap.json", tilemap);
                bool pellets_save_success = TileBoundGoodPelletsPoolSaveToFile("tileboundgoodpelletspool.json", pellets_pool);

                if (tilemap_save_success && pellets_save_success)
                {
                    imgui_tilemap_save_notification_text = imgui_tilemap_save_notification_text_success;
                }
                else
                {
                    imgui_tilemap_save_notification_text = imgui_tilemap_save_notification_text_failure;
                }
                printf("%s, %s", imgui_tilemap_save_notification_text, ctime(&save_timestamp));
                save_timestamp = time(NULL);
            }
        }
        ImGui::End();
    }


    void TilesetPropsPanelWindow(Tileset &tileset)
    {
        if (ImGui::Begin("Tileset Properties"))
        {

            ImGui::Text("Tile Set Properties");

            ImGui::Text("%s", assets_rel_path_prefix.string().c_str());
            ImGui::SameLine();
            ImGui::InputText(" ", &input_text_image_file_path);
            ImGui::SameLine();


            if (ImGui::Button("Load Tileset"))
            {
                bool success = TilesetCreateNewTextureandTileset(Global::renderer, tileset, assets_rel_path_prefix.string() + input_text_image_file_path, imgui_tileset_n_rows, imgui_tileset_n_cols);

                if (!success)
                {
                    ImGui::OpenPopup("TilesetLoadError");
                }
                else
                {
                    tileset_width = tileset.sprite_sheet.texture_w;
                    tileset_height = tileset.sprite_sheet.texture_h;
                }
            }

            ImGui::Text("Texture Width: %d\tTexture Height:%d", tileset_width, tileset_height);
            ImGui::Text("Tileset Mouse Pos:  (%d, %d)", imgui_tileset_window_mouse_x, imgui_tileset_window_mouse_y);
            ImGui::Text("Tileset Tile Width: %f\t Tile Height: %f)", tileset.sprite_sheet.cell_width(), tileset.sprite_sheet.cell_height());
            ImGui::InputInt("Tileset Rows", (int *) &imgui_tileset_n_rows);
            ImGui::InputInt("Tileset Cols", (int *) &imgui_tileset_n_cols);
            if (ImGui::Button("Resize Tileset Row/Col Dimensions"))
            {
                TilesetResizeandShiftValues(
                        tileset,
                        imgui_tileset_n_rows,
                        imgui_tileset_n_cols);
            }


            if (ImGui::Button("Save Tileset"))
            {
                bool success = TilesetSaveToFile("assets/tileset.json", tileset);
                if (success)
                {
                    imgui_tileset_save_notification_text = imgui_tileset_save_notification_text_success;
                }
                else
                {
                    imgui_tileset_save_notification_text = imgui_tileset_save_notification_text_failure;
                }
                printf("%s, %s", imgui_tileset_save_notification_text, ctime(&save_timestamp));
                save_timestamp = time(NULL);
            }


            if (ImGui::Button("Save Tileset"))
            {
                bool success = TilesetSaveToFile("tileset.json", tileset);
                if (success)
                {
                    imgui_tileset_save_notification_text = imgui_tileset_save_notification_text_success;
                }
                else
                {
                    imgui_tileset_save_notification_text = imgui_tileset_save_notification_text_failure;
                }
                printf("%s, %s", imgui_tileset_save_notification_text, ctime(&save_timestamp));
                save_timestamp = time(NULL);
            }
        }
        ImGui::End();
    }
}