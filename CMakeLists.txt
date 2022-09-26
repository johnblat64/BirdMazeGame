cmake_minimum_required(VERSION 3.23)
project(BirdMazeGame)


add_executable(Game
        src/game/_main.cpp src/engine/global.cpp src/engine/tile/tilemap_util.cpp src/engine/tile/tilemap_init.cpp src/engine/util/util_draw.cpp src/engine/util/util_load_save.cpp src/engine/util/util_error_handling.cpp src/engine/sprite/sprite_sheet.cpp)

add_executable(Editor
        src/editor/_main.cpp src/editor/editor.cpp src/engine/global.cpp src/engine/tile/tilemap_util.cpp src/engine/tile/tilemap_init.cpp src/engine/util/util_draw.cpp external/imgui/imgui.cpp external/imgui/imgui_demo.cpp external/imgui/imgui_draw.cpp external/imgui/imgui_tables.cpp external/imgui/imgui_widgets.cpp external/imgui/backends/imgui_impl_sdl.cpp  external/imgui/backends/imgui_impl_sdlrenderer.cpp src/engine/util/util_load_save.cpp src/engine/util/util_error_handling.cpp src/engine/sprite/sprite_sheet.cpp)

set(CMAKE_BUILD_RPATH_USE_ORIGIN  TRUE)
set_property(
    TARGET Game Editor
    PROPERTY BUILD_RPATH $ORIGIN/external/SDL2;$ORIGIN/external/flecs
)



add_subdirectory(external/SDL2)


include_directories(
    PRIVATE external
    PRIVATE external/imgui
    PRIVATE ${PROJECT_SOURCE_DIR}
)


target_link_directories(
        Game
        PRIVATE external/SDL2/lib
        PRIVATE external/flecs/Debug
)


target_link_directories(
        Editor
        PRIVATE external/SDL2/lib
        PRIVATE external/flecs/Debug
)

if(MSVC)
    add_definitions(-DMSVC)
endif()


target_link_libraries(Game SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(Editor SDL2::SDL2 SDL2::SDL2main)
