cmake_minimum_required(VERSION 3.28.0)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

project(gelly_installer)

add_subdirectory(vendor/SDL)

add_executable(gelly_installer WIN32
        src/main.cpp
        src/window.cpp
        src/window.hpp
        vendor/imgui/imgui.cpp
        vendor/imgui/imgui_draw.cpp
        vendor/imgui/imgui_widgets.cpp
        vendor/imgui/imgui_tables.cpp
        vendor/imgui/imgui_demo.cpp
        vendor/imgui/backends/imgui_impl_sdl2.cpp
        vendor/imgui/backends/imgui_impl_sdlrenderer2.cpp
        src/gui.cpp
        src/gui.hpp
        src/gui/main-installer-window.cpp
        src/gui/main-installer-window.hpp
)

target_link_libraries(gelly_installer
        SDL2-static
        SDL2main
)

target_include_directories(gelly_installer
        PRIVATE
        src
        vendor/SDL/include
        vendor/imgui
)

target_compile_definitions(gelly_installer
        PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

# copy Roboto-Medium.ttf from imgui/misc/fonts to the build directory
add_custom_command(TARGET gelly_installer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/misc/fonts/Roboto-Medium.ttf
        $<TARGET_FILE_DIR:gelly_installer>
)