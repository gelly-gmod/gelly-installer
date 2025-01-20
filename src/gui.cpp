#include "gui.hpp"

#include "logging/log.hpp"

#include <SDL.h>

namespace gelly {
GUI::GUI(std::shared_ptr<Window> window, std::shared_ptr<Curl> curl,
         bool autoUpdate)
    : window(window), curl(curl),
      mainInstallerWindow(window, curl, autoUpdate) {}

GUI::~GUI() {}

void GUI::UpdateCounter() {
  lastCounter = currentCounter;
  currentCounter = SDL_GetPerformanceCounter();
}

float GUI::GetDeltaTime() const {
  return (static_cast<float>(currentCounter - lastCounter) * 1000.f) /
         static_cast<float>(SDL_GetPerformanceFrequency());
}

bool GUI::RunFrame() {
  Clay_Vector2 scrollDelta = {};
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      return false;
    }

    if (event.type == SDL_MOUSEWHEEL) {
      scrollDelta.x = event.wheel.x;
      scrollDelta.y = event.wheel.y;
    }
  }

  UpdateCounter();

  int screenWidth = 0;
  int screenHeight = 0;
  SDL_GetWindowSize(window->GetWindow(), &screenWidth, &screenHeight);

  Clay_SetLayoutDimensions({.width = static_cast<float>(screenWidth),
                            .height = static_cast<float>(screenHeight)});

  int mouseX = 0;
  int mouseY = 0;
  const auto mouseState = SDL_GetMouseState(&mouseX, &mouseY);
  Clay_SetPointerState(
      {
          .x = static_cast<float>(mouseX),
          .y = static_cast<float>(mouseY),
      },
      mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));

  if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_D]) {
    debugEnabled = !debugEnabled;
    Clay_SetDebugModeEnabled(debugEnabled);
  }

  Clay_UpdateScrollContainers(true, scrollDelta, GetDeltaTime());

  Clay_BeginLayout();
  mainInstallerWindow.Render();
  auto renderCommands = Clay_EndLayout();

  window->RenderCommands(renderCommands);

  return true;
}
} // namespace gelly