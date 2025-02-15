#pragma once

#include <SDL.h>
#include <clay.h>
#include <vector>

namespace gelly {
enum class FontId { Body16, Header32, Button30, Button24, Version48 };

inline constexpr uint32_t FONT_ID(FontId id) {
  return static_cast<uint32_t>(id);
}

class Window {
public:
  Window();
  ~Window();

  template <typename FrameHandler>
  void RunEventLoop(FrameHandler frameHandler) {
    while (!ShouldClose()) {
      if (reinitializeClay) {
        ReinitializeClay();
      }

      if (!frameHandler()) {
        break;
      }
    }
  }

  void RenderCommands(const Clay_RenderCommandArray &array);

  SDL_Window *GetWindow() const { return window; }
  SDL_Renderer *GetRenderer() const { return renderer; }

private:
  SDL_Window *window;
  SDL_Renderer *renderer;

  static bool reinitializeClay;
  static void HandleClayErrors(Clay_ErrorData error);

  bool ShouldClose() const;
  void ReinitializeClay();

  static void RegisterFont(FontId &&font, const char *fontPath, int fontSize);
};

} // namespace gelly
