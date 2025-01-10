#define CLAY_IMPLEMENTATION
#include "window.hpp"
#include "clay.h"
#include "renderers/raylib/clay_renderer_raylib.c"
#include <cstdio>
#include <iostream>
#include <ostream>

namespace gelly {
namespace {
constexpr auto WINDOW_WIDTH = 1366;
constexpr auto WINDOW_HEIGHT = 768;
constexpr auto WINDOW_TITLE = "Gelly Installer";
} // namespace

bool Window::reinitializeClay = false;

Window::Window() {
  Clay_Raylib_Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE,
                         FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI |
                             FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
  ReinitializeClay();
  Clay_SetMeasureTextFunction(Raylib_MeasureText);
  Clay_SetDebugModeEnabled(false);
  Raylib_fonts[FONT_ID_BODY_16] = {
      .font = LoadFontEx("Roboto-Medium.ttf", 32, 0, 250),
      .fontId = FONT_ID_BODY_16};
  SetTextureFilter(Raylib_fonts[FONT_ID_BODY_16].font.texture,
                   TEXTURE_FILTER_BILINEAR);
}

void Window::RenderCommands(const Clay_RenderCommandArray &array) {
  Clay_Raylib_Render(array);
}

Window::~Window() {
  // I really don't know why but there is no matching "deinitialize" function
  // or something like that in Clay.
}

bool Window::ShouldClose() const { return WindowShouldClose(); }

void Window::ReinitializeClay() {
  auto requiredMemory = Clay_MinMemorySize();
  auto arena = Clay_CreateArenaWithCapacityAndMemory(requiredMemory,
                                                     malloc(requiredMemory));
  Clay_Initialize(arena,
                  {.width = static_cast<float>(GetScreenWidth()),
                   .height = static_cast<float>(GetScreenHeight())},
                  static_cast<Clay_ErrorHandler>(HandleClayErrors));
  Clay_GetCurrentContext()->debugSelectedElementId = 0;

  Clay_SetMeasureTextFunction(Raylib_MeasureText);
  Clay_SetDebugModeEnabled(false);
  Clay_SetExternalScrollHandlingEnabled(false);

  reinitializeClay = false;
}

void Window::HandleClayErrors(Clay_ErrorData error) {
  std::cerr << "Clay error: " << error.errorText.chars << std::endl;

  switch (error.errorType) {
  case CLAY_ERROR_TYPE_ARENA_CAPACITY_EXCEEDED:
    Clay_SetMaxElementCount(Clay_GetMaxElementCount() * 2);
    reinitializeClay = true;
    break;
  case CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED:
    Clay_SetMaxMeasureTextCacheWordCount(
        Clay_GetMaxMeasureTextCacheWordCount() * 2);
    reinitializeClay = true;
    break;
  default:
    std::cerr << "Unhandled error type" << std::endl;
    break;
  }
}

} // namespace gelly