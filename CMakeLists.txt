cmake_minimum_required(VERSION 3.28.0)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
cmake_policy(SET CMP0077 NEW) # important, this prevents older libraries such as libzip from using shared libraries

project(gelly_installer)

add_subdirectory(vendor/SDL)
add_subdirectory(vendor/VDFParser)
add_subdirectory(vendor/json)
add_subdirectory(vendor/libzip)

find_package(CURL REQUIRED)

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
        src/helpers/find-steam-directory.cpp
        src/helpers/find-steam-directory.hpp
        src/helpers/find-gmod-directory.cpp
        src/helpers/find-gmod-directory.hpp
        src/install/detect-gelly-installation.cpp
        src/install/detect-gelly-installation.hpp
        src/install/get-latest-gelly.cpp
        src/install/get-latest-gelly.hpp
        src/curl.cpp
        src/curl.hpp
        src/install/install-gelly.cpp
        src/install/install-gelly.hpp
        src/install/uninstall-gelly.cpp
        src/install/uninstall-gelly.hpp
        src/gui/popups/outdated-gelly.cpp
        src/gui/popups/uninstall-gelly.cpp
        src/gui/popups/fatal-error.cpp
        src/helpers/launch-gmod.cpp
        src/helpers/launch-gmod.hpp
)

target_link_libraries(gelly_installer
        SDL2-static
        SDL2main
        VDFParser
        CURL::libcurl
        libzip::zip
        wldap32
        ws2_32
)

target_include_directories(gelly_installer
        PRIVATE
        src
        vendor/SDL/include
        vendor/VDFParser
        vendor/imgui
        vendor/json/include
)

target_compile_definitions(gelly_installer
        PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

include(ExtraFiles)
get_extra_files(${CMAKE_CURRENT_SOURCE_DIR})

foreach (file ${EXTRA_FILES})
  add_custom_command(TARGET gelly_installer POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${file} $<TARGET_FILE_DIR:gelly_installer>)
endforeach ()

if (GELLY_CREATE_RELEASE)
  message(STATUS "CI: Will create release artifact")

  add_custom_command(TARGET gelly_installer POST_BUILD
          COMMAND ${CMAKE_COMMAND}
          -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
          -DCMAKE_CURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}
          -DCURL_INCLUDE_DIRS=${CURL_INCLUDE_DIRS}
          -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
          -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CreateReleaseArtifact.cmake
  )
endif ()

