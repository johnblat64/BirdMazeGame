//
// Created by frog on 9/19/22.
//

#ifndef BIRDMAZEGAMEEDITOR_EDITOR_H
#define BIRDMAZEGAMEEDITOR_EDITOR_H
//
// Created by frog on 9/19/22.
//
#include "src/engine/tile/tilemap.h"
#include "imgui/imgui.h"


namespace Editor
{
    extern ImGuiID dockspace_id;
    //----------------------------------------------------------
    void Setup(Tilemap &tilemap);

    //--------------------------------------------------------
    void
    DockingLayoutPreset(ImGuiID dockspace_id);

    //--------------------------------------------------------
    void EditorWindow(Tilemap &tilemap);
}
#endif //BIRDMAZEGAMEEDITOR_EDITOR_H
