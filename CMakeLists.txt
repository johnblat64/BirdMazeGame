cmake_minimum_required(VERSION 3.23)
project(BirdMazeGame)


add_executable(Game
        src/game/_main.cpp src/global.cpp src/tile/tilemap_util.cpp src/tile/tilemap_init.cpp src/util/util_draw.cpp src/util/util_load_save.cpp src/util/util_error_handling.cpp src/sprite/sprite_sheet.cpp src/util/util_misc.h src/util/util_misc.cpp src/pellet_pools/tile_bound_good_pellets_pool.cpp src/pellet_pools/moving_pellets_pool.cpp src/tile/tileset.h)

add_executable(Editor
        src/editor/_main.cpp src/editor/editor.cpp src/global.cpp src/tile/tilemap_util.cpp src/tile/tilemap_init.cpp src/util/util_draw.cpp external/imgui/misc/cpp/imgui_stdlib.cpp external/imgui/imgui.cpp external/imgui/imgui_demo.cpp external/imgui/imgui_draw.cpp external/imgui/imgui_tables.cpp external/imgui/imgui_widgets.cpp external/imgui/backends/imgui_impl_sdl.cpp  external/imgui/backends/imgui_impl_sdlrenderer.cpp src/util/util_load_save.cpp src/util/util_error_handling.cpp src/sprite/sprite_sheet.cpp src/util/util_misc.h src/util/util_misc.cpp src/util/util_panning.cpp src/pellet_pools/tile_bound_good_pellets_pool.cpp src/pellet_pools/tile_bound_good_pellets_pool.h src/pellet_pools/moving_pellets_pool.h src/pellet_pools/moving_pellets_pool.cpp src/tile/tileset.h src/tile/tileset_util.cpp)

set(CMAKE_BUILD_RPATH_USE_ORIGIN  TRUE)
set_property(
    TARGET Game Editor
    PROPERTY BUILD_RPATH $ORIGIN/external/SDL2;
)

set_property(
    TARGET Game Editor
    PROPERTY CXX_STANDARD 17
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
        PRIVATE external/SDL2
)


target_link_directories(
        Editor
        PRIVATE external/SDL2/lib
        PRIVATE external/flecs/Debug
        PRIVATE external/SDL2
)

# file(REMOVE_RECURSE  ${CMAKE_BINARY_DIR}/assets)


file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

if(MSVC)
    add_definitions(-DMSVC)
    add_custom_command(TARGET Editor POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${CMAKE_BINARY_DIR}/external/SDL2/SDL2d.dll"      # <--this is in-file
        "${CMAKE_BINARY_DIR}/SDL2d.dll"


    )                 # <--this is out-file path

endif()


target_link_libraries(Game SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(Editor SDL2::SDL2 SDL2::SDL2main)
