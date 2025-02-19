// clang-format off
#undef NOUSER
#include <windows.h>
#include <shellapi.h>
// clang-format on

#include "app/config.hpp"
#include "app/relocate-installation.hpp"
#include "app/setup-uri-handler.hpp"
#include "gui.hpp"
#include "helpers/detect-elevation.hpp"
#include "helpers/launch-installer.hpp"
#include "logging/crash-logging.hpp"
#include "logging/log.hpp"
#include "window.hpp"

#include <cstdio>
#include <memory>

namespace {
constexpr auto AUTO_UPDATE_ARG = "autoupdate";
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
            int nCmdShow) {
  auto argc = 0;
  auto argvWideChar = CommandLineToArgvW(GetCommandLineW(), &argc);

  auto argv = new char *[argc];
  for (auto i = 0; i < argc; ++i) {
    const auto length = wcslen(argvWideChar[i]);
    argv[i] = new char[length + 1];
    wcstombs(argv[i], argvWideChar[i], length);
    argv[i][length] = '\0';
  }

#ifndef _DEBUG
  if (!gelly::Config::IsURIHandlerRegistered() &&
      gelly::Config::IsAppInstalled()) {
    gelly::Log::Info("URI handler not registered, attempting to register");
    gelly::SetupURIHandler();
  }

  if (!gelly::Config::IsAppUpToDate()) {
    // Install this version to the directory
    const auto newPath = gelly::RelocateInstallation();
    gelly::Config::SetAppInstallPath(newPath);
    gelly::Config::SetAppVersion(gelly::Config::APP_VERSION);

    gelly::helpers::LaunchInstaller(newPath, false, argc, argv);
    return 0; // We already spawn from the new location anyway
  } else {
    const auto cwd = std::filesystem::current_path();
    const auto installPath =
        std::filesystem::path(*gelly::Config::GetAppInstallPath());

    auto cwdString = cwd.generic_string();
    auto installPathString = installPath.generic_string();

    if (cwdString != installPathString) {
      gelly::helpers::LaunchInstaller(
          gelly::Config::GetAppInstallPath().value(), false, argc, argv);
      return 0;
    }
  }
#endif

  auto autoUpdate = false;
  if (argc > 1) {
    // Since we also invoke auto updates using URI handlers, we find a substring
    // instead of an exact match
    autoUpdate = strstr(argv[1], AUTO_UPDATE_ARG) != nullptr;
  }

  gelly::SetupCrashLogging();
  gelly::Log::Info("Starting Gelly Installer");
  gelly::Log::Info("Auto update: {}", autoUpdate ? "yes" : "no");
  gelly::Log::Info("Arguments: {}", argc);
  if (argc > 0) {
    for (auto i = 0; i < argc; ++i) {
      gelly::Log::Info("Argument {}: {}", i, argv[i]);
    }
  }

  auto curl = std::make_shared<gelly::Curl>();
  auto window = std::make_shared<gelly::Window>();
  const auto gui = std::make_shared<gelly::GUI>(window, curl, autoUpdate);
  gelly::Log::Info("GUI initialized, starting event loop");

  window->RunEventLoop([&] {
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