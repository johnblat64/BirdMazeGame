cmake_minimum_required(VERSION 3.23)
project(BirdMazeGame)

#set(CMAKE_INSTALL_PREFIX SDL)

add_executable(BirdMazeGame
        src/game/_main.cpp src/engine/tile/tilemap_util.cpp src/engine/tile/tilemap_init.cpp src/engine/util/util_draw.cpp external/imgui/imgui.cpp external/imgui/imgui_demo.cpp external/imgui/imgui_draw.cpp external/imgui/imgui_tables.cpp external/imgui/imgui_widgets.cpp external/imgui/backends/imgui_impl_sdl.cpp  external/imgui/backends/imgui_impl_sdlrenderer.cpp)

set(CMAKE_BUILD_RPATH_USE_ORIGIN  TRUE)

set_property(
    TARGET ${PROJECT_NAME}
    PROPERTY BUILD_RPATH $ORIGIN/external/SDL2;$ORIGIN/external/flecs
)

add_subdirectory(external/SDL2 EXCLUDE_FROM_ALL)


add_subdirectory(external/flecs)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC external
    PUBLIC external/imgui
)

target_link_directories(
    ${PROJECT_NAME}
    PUBLIC external/SDL2/lib
    PUBLIC external/flecs/Debug
)

if (WIN32)
set(FLECSLIB flecs::flecs_static)
else()
set(FLECSLIB flecs::flecs)
endif()

target_link_libraries(BirdMazeGame SDL2::SDL2 SDL2::SDL2main ${FLECSLIB})
