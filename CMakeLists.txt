cmake_minimum_required(VERSION 3.28.0)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
# we use a global policy because every target should try to statically link
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

project(gelly_installer)

add_subdirectory(vendor/VDFParser)
add_subdirectory(vendor/json)
add_subdirectory(vendor/libzip)
add_subdirectory(vendor/raylib)

find_package(CURL REQUIRED)
find_program(RCEDIT rcedit-x64 rcedit)

add_executable(gelly_installer WIN32
        src/main.cpp
        src/window.cpp
        src/window.hpp
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
        src/helpers/launch-gmod.cpp
        src/helpers/launch-gmod.hpp
        src/logging/log.hpp
        src/logging/log.cpp
        src/logging/crash-logging.cpp
        src/logging/crash-logging.hpp
        src/helpers/find-app-data.cpp
        src/helpers/find-app-data.hpp
        src/app/config.cpp
        src/app/config.hpp
        src/app/relocate-installation.cpp
        src/app/relocate-installation.hpp
        src/helpers/launch-installer.cpp
        src/helpers/launch-installer.hpp
        src/helpers/parse-version.cpp
        src/helpers/parse-version.hpp
        src/app/setup-uri-handler.cpp
        src/app/setup-uri-handler.hpp
        src/app/registry.cpp
        src/app/registry.hpp
        src/helpers/detect-elevation.cpp
        src/helpers/detect-elevation.hpp
        src/helpers/clay-dynamic-string.hpp
        src/helpers/release-markdown.cpp
        src/helpers/release-markdown.hpp
)

target_link_libraries(gelly_installer
        VDFParser
        raylib_static
        CURL::libcurl
        libzip::zip
        wldap32
        ws2_32
        version
)

target_include_directories(gelly_installer
        PRIVATE
        src
        ${CMAKE_CURRENT_BINARY_DIR}/src
        vendor/VDFParser
        vendor/imgui
        vendor/raylib/src
        vendor/clay
        vendor/json/include
)

target_compile_definitions(gelly_installer
        PRIVATE
        WIN32_LEAN_AND_MEAN
        # Removes a lot of conflicting definitions from windows.h
        NOGDI
        NOUSER
        NOMINMAX
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(gelly_installer
          PRIVATE
          CLAY_DEBUG
  )
endif ()

include(ExtraFiles)
include(GetCurrentVersion)
get_extra_files(${CMAKE_CURRENT_SOURCE_DIR})
get_current_version_from_changelog(${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG.md)
configure_file(src/version.hpp.in src/version.hpp @ONLY)

foreach (file ${EXTRA_FILES})
  add_custom_command(TARGET gelly_installer POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${file} $<TARGET_FILE_DIR:gelly_installer>)
endforeach ()

if (RCEDIT)
  message(STATUS "Will set icon for gelly_installer.exe")
  add_custom_command(TARGET gelly_installer POST_BUILD
          COMMAND ${RCEDIT}
          $<TARGET_FILE:gelly_installer>
          --set-icon ${CMAKE_CURRENT_SOURCE_DIR}/branding/gelly_installer.ico --set-version-string "ProductName" "Gelly Installer"
  )
else ()
  message(WARNING "Could not find rcedit, will not set icon for gelly_installer.exe")
endif ()

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