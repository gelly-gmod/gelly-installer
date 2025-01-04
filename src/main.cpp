#include "app/config.hpp"
#include "app/relocate-installation.hpp"
#include "gui.hpp"
#include "helpers/launch-installer.hpp"
#include "logging/crash-logging.hpp"
#include "logging/log.hpp"
#include "window.hpp"

#include <SDL.h>
#include <cstdio>
#include <memory>

namespace {
constexpr auto AUTO_UPDATE_ARG = "autoupdate";
}

int main(int argc, char *argv[]) {
  if (!gelly::Config::IsAppUpToDate()) {
    // Install this version to the directory
    const auto newPath = gelly::RelocateInstallation();
    gelly::Config::SetAppInstallPath(newPath);
    gelly::Config::SetAppVersion(gelly::Config::APP_VERSION);

    gelly::helpers::LaunchInstaller(newPath);
    return 0; // We already spawn from the new location anyway
  } else {
    const auto cwd = std::filesystem::current_path();
    const auto installPath =
        std::filesystem::path(*gelly::Config::GetAppInstallPath());

    auto cwdString = cwd.generic_string();
    auto installPathString = installPath.generic_string();

    if (cwdString != installPathString) {
      gelly::helpers::LaunchInstaller(
          gelly::Config::GetAppInstallPath().value());
      return 0;
    }
  }

  auto autoUpdate = false;
  if (argc > 1) {
    // Since we also invoke auto updates using URI handlers, we find a substring
    // instead of an exact match
    autoUpdate = strstr(argv[1], AUTO_UPDATE_ARG) != nullptr;
  }

  gelly::SetupCrashLogging();
  gelly::Log::Info("Starting Gelly Installer");

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  gelly::Log::Info("SDL initialized, starting GUI");
  auto curl = std::make_shared<gelly::Curl>();
  auto window = std::make_shared<gelly::Window>();
  const auto gui = std::make_shared<gelly::GUI>(window, curl, autoUpdate);
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