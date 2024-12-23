#include "gui.hpp"
#include "window.hpp"

#include <SDL.h>
#include <cstdio>
#include <memory>

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  auto curl = std::make_shared<gelly::Curl>();
  auto window = std::make_shared<gelly::Window>();
  const auto gui = std::make_shared<gelly::GUI>(window, curl);

  window->RunEventLoop(
      [&](SDL_Event &ev) {
        if (ev.type == SDL_QUIT) {
          return false;
        }

        gui->ProcessSDLEvent(ev);
        return true;
      },
      [&] {
        if (!gui->RunFrame()) {
          return false;
        }

        return true;
      });

  return 0;
}