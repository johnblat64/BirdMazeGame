cmake_minimum_required(VERSION 3.23)
project(BirdMazeGame)


add_executable(BirdMazeGame
        src/game/_main.cpp src/engine/tile/tilemap_util.cpp src/engine/tile/tilemap_init.cpp src/engine/util/util_draw.cpp external/imgui/imgui.cpp external/imgui/imgui_demo.cpp external/imgui/imgui_draw.cpp external/imgui/imgui_tables.cpp external/imgui/imgui_widgets.cpp external/imgui/backends/imgui_impl_sdl.cpp  external/imgui/backends/imgui_impl_sdlrenderer.cpp src/engine/util/util_load_save.cpp src/engine/util/util_error_handling.cpp)

set(CMAKE_BUILD_RPATH_USE_ORIGIN  TRUE)
set_property(
    TARGET ${PROJECT_NAME}
    PROPERTY BUILD_RPATH $ORIGIN/external/SDL2;$ORIGIN/external/flecs
)

add_subdirectory(external/SDL2 EXCLUDE_FROM_ALL)


target_include_directories(
    ${PROJECT_NAME}
    PRIVATE external
    PRIVATE external/imgui
    PRIVATE ${PROJECT_SOURCE_DIR}
)

target_link_directories(
    ${PROJECT_NAME}
    PRIVATE external/SDL2/lib
    PRIVATE external/flecs/Debug
)

if(MSVC)
    add_definitions(-DMSVC)
endif()


target_link_libraries(BirdMazeGame SDL2::SDL2 SDL2::SDL2main)
