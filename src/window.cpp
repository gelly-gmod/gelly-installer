#define CLAY_IMPLEMENTATION
#include "window.hpp"
#include "clay.h"
#include "logging/log.hpp"
#include "renderer/clay_sdl2_renderer.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

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
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    Log::Error("Failed to initialize SDL: {}", SDL_GetError());
    return;
  }

  if (TTF_Init() != 0) {
    Log::Error("Failed to initialize SDL_ttf: {}", TTF_GetError());
    return;
  }

  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    Log::Error("Failed to initialize SDL_image: {}", IMG_GetError());
    return;
  }

  if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window,
                                  &renderer) != 0) {
    Log::Error("Failed to create window and renderer: {}", SDL_GetError());
    return;
  }

  SDL_SetWindowTitle(window, WINDOW_TITLE);

  SDL_RendererInfo info = {};
  SDL_GetRendererInfo(renderer, &info);

  Log::Info("Running SDL2 with renderer: {}", info.name);

  ReinitializeClay();
  Clay_SetMeasureTextFunction(renderer::SDL2_MeasureText);

  Clay_SetDebugModeEnabled(false);
  RegisterFont(FontId::Body16, "Poppins-Medium.ttf", 16);
  RegisterFont(FontId::Header32, "Poppins-Bold.ttf", 32);
  RegisterFont(FontId::Button30, "Poppins-Bold.ttf", 30);
  RegisterFont(FontId::Button24, "Poppins-Bold.ttf", 24);
  RegisterFont(FontId::Version48, "Poppins-Bold.ttf", 48);
}

void Window::RenderCommands(const Clay_RenderCommandArray &array) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  renderer::Clay_SDL2_Render(renderer, array);

  SDL_RenderPresent(renderer);
}

Window::~Window() {
  // I really don't know why but there is no matching "deinitialize" function
  // or something like that in Clay.
}

bool Window::ShouldClose() const { return false; }

void Window::ReinitializeClay() {
  auto requiredMemory = Clay_MinMemorySize();
  auto arena = Clay_CreateArenaWithCapacityAndMemory(requiredMemory,
                                                     malloc(requiredMemory));

  int screenWidth = 0;
  int screenHeight = 0;
  SDL_GetWindowSize(window, &screenWidth, &screenHeight);

  Clay_Initialize(arena,
                  {.width = static_cast<float>(screenWidth),
                   .height = static_cast<float>(screenHeight)},
                  static_cast<Clay_ErrorHandler>(HandleClayErrors));
  Clay_GetCurrentContext()->debugSelectedElementId = 0;

  Clay_SetMeasureTextFunction(renderer::SDL2_MeasureText);
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
        Clay_GetMaxMeasureTextCacheWordCount() * 4);
    reinitializeClay = true;
    break;
  default:
    std::cerr << "Unhandled error type" << std::endl;
    break;
  }
}

void Window::RegisterFont(FontId &&font, const char *fontPath, int fontSize) {
  if (!renderer::RegisterFont(fontPath, fontSize, FONT_ID(font))) {
    Log::Error("Failed to load font {}: {}", fontPath, TTF_GetError());
  } else {
    Log::Info("Loaded font {} at {}px", fontPath, fontSize);
  }
}

} // namespace gelly