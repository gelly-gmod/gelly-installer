#include "window.hpp"

#include <cstdio>

namespace gelly {
namespace {
constexpr auto WINDOW_WIDTH = 1366;
constexpr auto WINDOW_HEIGHT = 768;
constexpr auto WINDOW_TITLE = "Gelly Installer";
} // namespace

Window::Window() : window(CreateWindow()), renderer(CreateRenderer(window)) {}
Window::~Window() {
  if (renderer) {
    SDL_DestroyRenderer(renderer);
  }

  if (window) {
    SDL_DestroyWindow(window);
  }
}

SDL_Window *Window::CreateWindow() {
  SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                                        WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
    return nullptr;
  }

  return window;
}

SDL_Renderer *Window::CreateRenderer(SDL_Window *window) {
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return nullptr;
  }

  return renderer;
}

} // namespace gelly