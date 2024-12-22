#pragma once
#include <SDL_events.h>
#include <SDL_render.h>

namespace gelly {
class Window {
public:
  Window();
  ~Window();

  template <typename InputHandler, typename FrameHandler>
  void RunEventLoop(InputHandler inputHandler, FrameHandler frameHandler) {
    bool running = true;
    SDL_Event ev;

    while (running) {
      while (SDL_PollEvent(&ev)) {
        if (!inputHandler(ev)) {
          running = false;
        }
      }

      if (!frameHandler()) {
        break;
      }
    }
  }

  SDL_Window *GetWindow() const { return window; }
  SDL_Renderer *GetRenderer() const { return renderer; }

private:
  SDL_Window *window;
  SDL_Renderer *renderer;

  static SDL_Window *CreateWindow();
  static SDL_Renderer *CreateRenderer(SDL_Window *window);
};

} // namespace gelly
