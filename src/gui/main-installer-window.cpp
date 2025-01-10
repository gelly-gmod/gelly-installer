#include "main-installer-window.hpp"

#include "helpers/find-gmod-directory.hpp"
#include "helpers/find-steam-directory.hpp"
#include "helpers/launch-gmod.hpp"
#include "install/detect-gelly-installation.hpp"
#include "install/get-latest-gelly.hpp"
#include "install/install-gelly.hpp"
#include "install/uninstall-gelly.hpp"
#include "logging/log.hpp"
#include "window.hpp"

#include <clay.h>
#include <imgui.h>

namespace gelly {
namespace {
constexpr auto HEADER = "Gelly Installer";
}

MainInstallerWindow::MainInstallerWindow(std::shared_ptr<Curl> curl,
                                         bool autoUpdate)
    : curl(std::move(curl)), latestGellyInfo(GetLatestGellyInfo(this->curl)) {
  Log::Info("Latest Gelly version: {}",
            latestGellyInfo.has_value() ? latestGellyInfo->version : "N/A");

  DetectGellyInstallation();
  Log::Info("Gelly installation detected: {}",
            gellyInstallation.has_value() ? "yes" : "no");
  Log::Info("Gelly version: {}",
            gellyInstallation.has_value() ? gellyInstallation->version : "N/A");

  // It's called auto-update but we also prefer installing Gelly if it's not
  // installed already
  if (autoUpdate && latestGellyInfo.has_value()) {
    try {
      InstallGelly(*latestGellyInfo, this->curl, gellyInstallation);
      DetectGellyInstallation();
      LaunchGMod();
    } catch (const std::exception &e) {
    }
  }
}

void MainInstallerWindow::Render() {
  auto expandLayout =
      Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};

  auto background = Clay_RectangleElementConfig{.color = {20, 20, 20, 255},
                                                .cornerRadius = 8};

  CLAY(CLAY_ID("MainWindow"), CLAY_RECTANGLE(background),
       CLAY_LAYOUT({.layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = expandLayout,
                    .padding = {16, 16},
                    .childGap = 16})) {
    CLAY(CLAY_ID("HeaderRow"),
         CLAY_RECTANGLE({
             .color = {40, 40, 40, 255},
             .cornerRadius = 4,
         }),
         CLAY_LAYOUT({.layoutDirection = CLAY_LEFT_TO_RIGHT,
                      .sizing = {.width = CLAY_SIZING_GROW(0),
                                 .height = CLAY_SIZING_FIXED(48)},
                      .padding = {8, 8},
                      .childGap = 32})) {
      CLAY_TEXT(CLAY_STRING("Hi!!!"),
                CLAY_TEXT_CONFIG({.fontId = Window::FONT_ID_BODY_16,
                                  .fontSize = 32,
                                  .textColor = {255, 255, 255, 255}}));
      CLAY_TEXT(CLAY_STRING("Welcome..."),
                CLAY_TEXT_CONFIG({.fontId = Window::FONT_ID_BODY_16,
                                  .fontSize = 32,
                                  .textColor = {255, 255, 255, 255}}));
    }
  }
}

void MainInstallerWindow::DetectGellyInstallation() {
  const auto steamPath = helpers::FindSteamDirectory();
  if (!steamPath.has_value()) {
    return;
  }

  const auto gmodPath = helpers::FindGModDirectory(*steamPath);
  if (!gmodPath.has_value()) {
    return;
  }

  Log::Info("Garry's Mod directory: {}", gmodPath->string());
  Log::Info("Steam directory: {}", steamPath->string());
  gellyInstallation = gelly::DetectGellyInstallation(*gmodPath);
}

void MainInstallerWindow::CenterPopup() {}

} // namespace gelly