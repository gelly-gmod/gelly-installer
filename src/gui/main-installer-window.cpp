#include "main-installer-window.hpp"

#include "helpers/clay-dynamic-string.hpp"
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
#include <functional>
#include <imgui.h>
#include <iostream>

namespace gelly {
namespace {
constexpr auto HEADER = "Gelly Installer";
constexpr auto ButtonTextConfig = Clay_TextElementConfig{
    .fontId = FONT_ID(FontId::Button30),
    .fontSize = 30,
    .textColor = {255, 255, 255, 255},
};

constexpr auto DisabledButtonTextConfig = Clay_TextElementConfig{
    .fontId = FONT_ID(FontId::Button30),
    .fontSize = 30,
    .textColor = {128, 128, 128, 255},
};

constexpr auto FWButtonConfig = Clay_TextElementConfig{
    .fontId = FONT_ID(FontId::Button24),
    .fontSize = 24,
    .textColor = {255, 255, 255, 255},
};

constexpr auto DisabledFWButtonConfig = Clay_TextElementConfig{
    .fontId = FONT_ID(FontId::Button24),
    .fontSize = 24,
    .textColor = {128, 128, 128, 255},
};

MainInstallerWindow *mainInstallerWindow = nullptr;

void ButtonOnClickHandler(Clay_ElementId id, Clay_PointerData data,
                          intptr_t userdata) {
  if (data.state != CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    return;
  }

  auto function = reinterpret_cast<std::function<void()> *>(userdata);
  (*function)();
}

struct ButtonComponentProps {
  Clay_String text;
  std::function<void()> *onClick;
  bool disabled = false;
  Clay_Color color = {60, 60, 60, 255};
  Clay_Color hoverColor = {90, 90, 90, 255};
  Clay_TextElementConfig textConfig = ButtonTextConfig;
  Clay_TextElementConfig disabledTextConfig = DisabledButtonTextConfig;
  float widthPercent = 0.3;
  bool expandHeight = true;
};

void ButtonComponent(ButtonComponentProps props) {
  auto [text, function, disabled, color, hoverColor, textConfig,
        disabledTextConfig, widthPercent, expandHeight] = props;
  CLAY(CLAY_RECTANGLE({
           .color = Clay_Hovered() && !disabled ? hoverColor : color,
           .cornerRadius = 4,
       }),
       CLAY_LAYOUT({
           .layoutDirection = CLAY_LEFT_TO_RIGHT,
           .sizing = {.width = CLAY_SIZING_PERCENT(widthPercent),
                      .height = expandHeight ? CLAY_SIZING_GROW(0)
                                             : CLAY_SIZING_FIT()},
           .padding = {8, 8},
           .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                              .y = CLAY_ALIGN_Y_CENTER},
       }),
       Clay_OnHover(disabled ? nullptr : ButtonOnClickHandler,
                    reinterpret_cast<intptr_t>(function))) {
    CLAY_TEXT(text,
              CLAY_TEXT_CONFIG(disabled ? disabledTextConfig : textConfig));
  }
}

void FullWidthButtonComponent(Clay_String text, std::function<void()> *function,
                              bool disabled) {
  ButtonComponent({.text = text,
                   .onClick = function,
                   .disabled = disabled,
                   .textConfig = FWButtonConfig,
                   .disabledTextConfig = DisabledFWButtonConfig,
                   .widthPercent = 1,
                   .expandHeight = false});
}

/**
 * Does nothing but takes up space, useful for replicating auto-margin behavior
 */
void AutoGrowComponent() {
  CLAY(CLAY_LAYOUT({
      .layoutDirection = CLAY_LEFT_TO_RIGHT,
      .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT()},
      .padding = {0, 0},
  })){};
}
} // namespace

MainInstallerWindow::MainInstallerWindow(std::shared_ptr<Curl> curl,
                                         bool autoUpdate)
    : curl(std::move(curl)), latestGellyInfo(GetLatestGellyInfo(this->curl)),
      onInstallClick([this] { HandleOnInstallClick(); }),
      onLaunchClick([this] { HandleOnLaunchClick(); }),
      onUninstallClick([this] { HandleOnUninstallClick(); }) {
  mainInstallerWindow = this;
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

  if (latestGellyInfo.has_value()) {
    releaseMarkdown = helpers::ReleaseMarkdown(latestGellyInfo->changelog);
    installButtonString = helpers::CLAY_DYN_STRING(
        std::format("Install {}", latestGellyInfo->version));
    versionString =
        helpers::CLAY_DYN_STRING(std::move(latestGellyInfo->version));
  }
}

void MainInstallerWindow::Render() {
  auto expandLayout =
      Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};

  auto background = Clay_RectangleElementConfig{.color = {15, 15, 15, 255},
                                                .cornerRadius = 8};

  if (!latestGellyInfo.has_value()) {
    CLAY(CLAY_ID("MainWindow"), CLAY_RECTANGLE(background),
         CLAY_LAYOUT({.layoutDirection = CLAY_TOP_TO_BOTTOM,
                      .sizing = expandLayout,
                      .padding = {16, 16},
                      .childGap = 16})) {
      CLAY_TEXT(CLAY_STRING("Failed to fetch latest Gelly version"),
                CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID(FontId::Header32),
                    .fontSize = 32,
                    .textColor = {255, 255, 255, 255},
                }));
    }

    return;
  }

  CLAY(CLAY_ID("MainWindow"), CLAY_RECTANGLE(background),
       CLAY_LAYOUT({.layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = expandLayout,
                    .padding = {16, 16},
                    .childGap = 16})) {
    CLAY(CLAY_ID("HeaderRow"),
         CLAY_RECTANGLE({
             .color = {30, 15, 15, 255},
             .cornerRadius = 4,
         }),
         CLAY_LAYOUT({.layoutDirection = CLAY_LEFT_TO_RIGHT,
                      .sizing = {.width = CLAY_SIZING_GROW(0),
                                 .height = CLAY_SIZING_FIXED(48)},
                      .padding = {8, 8},
                      .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                         .y = CLAY_ALIGN_Y_CENTER},
                      .childGap = 32})) {
      CLAY_TEXT(CLAY_STRING("Gelly Installer"),
                CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID(FontId::Header32),
                    .fontSize = 32,
                    .textColor = {255, 255, 255, 255},
                }));
    }
    CLAY(CLAY_ID("Changelog"),
         CLAY_RECTANGLE({
             .color = {20, 20, 20, 255},
             .cornerRadius = 4,
         }),
         CLAY_LAYOUT({
             .layoutDirection = CLAY_TOP_TO_BOTTOM,
             .sizing = expandLayout,
             .padding = {8, 8},
             .childGap = 8,
         })) {
      CLAY_TEXT(versionString.string, CLAY_TEXT_CONFIG({
                                          .fontId = FONT_ID(FontId::Header32),
                                          .fontSize = 32,
                                          .textColor = {255, 100, 100, 255},
                                      }));
      releaseMarkdown.Render();
    }
    CLAY(CLAY_ID("FooterRow"),
         CLAY_RECTANGLE({
             .color = {30, 15, 15, 255},
             .cornerRadius = 4,
         }),
         CLAY_LAYOUT({
             .layoutDirection = CLAY_LEFT_TO_RIGHT,
             .sizing = {.width = CLAY_SIZING_GROW(0),
                        .height = CLAY_SIZING_FIXED(64)},
             .padding = {8, 8},
             .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                .y = CLAY_ALIGN_Y_CENTER},
             .childGap = 32,
         })) {

      AutoGrowComponent();

      if (gellyInstallation.has_value() &&
          !gellyInstallation->IsOutdated(latestGellyInfo->version)) {
        ButtonComponent({.text = launchButtonString.string,
                         .color = {35, 117, 26, 255},
                         .hoverColor = {50, 153, 38, 255},
                         .onClick = &onLaunchClick});
      } else {
        ButtonComponent({.text = installButtonString.string,
                         .color = {35, 117, 26, 255},
                         .hoverColor = {50, 153, 38, 255},
                         .onClick = &onInstallClick});
      }

      AutoGrowComponent();

      CLAY(CLAY_ID("UninstallButton"),
           CLAY_RECTANGLE({
               .color = {30, 15, 15, 255},
               .cornerRadius = 4,
           }),
           CLAY_LAYOUT({
               .layoutDirection = CLAY_TOP_TO_BOTTOM,
               .sizing = {.width = CLAY_SIZING_FIXED(128),
                          .height = CLAY_SIZING_FIT()},
               .padding = {0, 0},
               .childGap = 4,
               .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                  .y = CLAY_ALIGN_Y_CENTER},
           })) {
        CLAY_TEXT(usingVersionString.string,
                  CLAY_TEXT_CONFIG({
                      .fontId = FONT_ID(FontId::Body16),
                      .fontSize = 16,
                      .textColor = {255, 255, 255, 255},
                  }));
        FullWidthButtonComponent(CLAY_STRING("Uninstall"), &onUninstallClick,
                                 !gellyInstallation.has_value());
      }
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
  if (gellyInstallation.has_value()) {
    usingVersionString = helpers::CLAY_DYN_STRING(
        std::format("Using version {}", gellyInstallation->version));
    launchButtonString = helpers::CLAY_DYN_STRING(
        std::format("Launch {}", gellyInstallation->version));
  } else {
    usingVersionString = helpers::CLAY_DYN_STRING("Not installed");
    launchButtonString = helpers::CLAY_DYN_STRING("Launch");
  }
}

void MainInstallerWindow::CenterPopup() {}

void MainInstallerWindow::HandleOnInstallClick() {
  if (!latestGellyInfo.has_value()) {
    return;
  }

  try {
    InstallGelly(*latestGellyInfo, curl, gellyInstallation);
    DetectGellyInstallation();
  } catch (const std::exception &e) {
  }
}

void MainInstallerWindow::HandleOnLaunchClick() { LaunchGMod(); }

void MainInstallerWindow::HandleOnUninstallClick() {
  if (!gellyInstallation.has_value()) {
    return;
  }

  try {
    UninstallGelly(*gellyInstallation);
    DetectGellyInstallation();
  } catch (const std::exception &e) {
  }
}

} // namespace gelly