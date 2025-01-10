#include "gui.hpp"

#include "logging/log.hpp"

#include <raylib.h>

namespace gelly {
GUI::GUI(std::shared_ptr<Window> window, std::shared_ptr<Curl> curl,
         bool autoUpdate)
    : window(std::move(window)), curl(std::move(curl)),
      mainInstallerWindow(curl, autoUpdate) {}

GUI::~GUI() {}

bool GUI::RunFrame() {
  Clay_SetLayoutDimensions({.width = static_cast<float>(GetScreenWidth()),
                            .height = static_cast<float>(GetScreenHeight())});
  Clay_SetPointerState(
      {
          .x = static_cast<float>(GetMouseX()),
          .y = static_cast<float>(GetMouseY()),
      },
      IsMouseButtonDown(0));

  if (IsKeyPressed(KEY_D)) {
    debugEnabled = !debugEnabled;
    Clay_SetDebugModeEnabled(debugEnabled);
  }

  const auto scrollDelta = GetMouseWheelMoveV();
  Clay_UpdateScrollContainers(true, {.x = scrollDelta.x, .y = scrollDelta.y},
                              GetFrameTime());
  Clay_BeginLayout();
  mainInstallerWindow.Render();
  auto renderCommands = Clay_EndLayout();

  BeginDrawing();
  ClearBackground(BLACK);
  window->RenderCommands(renderCommands);
  EndDrawing();

  return true;
}
} // namespace gelly