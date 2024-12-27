#include "gui.hpp"
#include "logging/crash-logging.hpp"
#include "logging/log.hpp"
#include "window.hpp"

#include <SDL.h>
#include <cstdio>
#include <memory>

int main(int argc, char *argv[]) {
  gelly::SetupCrashLogging();
  gelly::Log::Info("Starting Gelly Installer");

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  gelly::Log::Info("SDL initialized, starting GUI");
  auto curl = std::make_shared<gelly::Curl>();
  auto window = std::make_shared<gelly::Window>();
  const auto gui = std::make_shared<gelly::GUI>(window, curl);
  gelly::Log::Info("GUI initialized, starting event loop");

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

  gelly::Log::Info("Event loop ended, shutting down GUI");
  gelly::Log::SaveToFile();
  gelly::ShutdownCrashLogging();
  return 0;
}