//
// Created by frog on 9/19/22.
//

#ifndef BIRDMAZEGAMEEDITOR_EDITOR_H
#define BIRDMAZEGAMEEDITOR_EDITOR_H
//
// Created by frog on 9/19/22.
//
#include "src/tile/tilemap.h"
#include "imgui/imgui.h"
#include <src/editor/level_data.h>


namespace Editor
{
    extern ImGuiID dockspace_id;

    //--------------------------------------------------------
    void
    DockingLayoutPreset(ImGuiID dockspace_id);

    //--------------------------------------------------------
    void EditorWindow(LevelData &level_data);
}
#endif //BIRDMAZEGAMEEDITOR_EDITOR_H
