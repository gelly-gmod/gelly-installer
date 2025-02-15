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
#include "renderer/clay_sdl2_renderer.hpp"
#include "window.hpp"

#include <clay.h>
#include <functional>
#include <imgui.h>
#include <iostream>

namespace gelly {
namespace {
constexpr auto HEADER = "Gelly Installer";
constexpr auto ButtonTextConfig = Clay_TextElementConfig{
    .fontId = FONT_ID(FontId::Button24),
    .fontSize = 24,
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

struct ComboButtonAction {
  Clay_String name;
  std::function<void()> *action;
};

struct ComboButtonProps {
  Clay_Sizing sizing;
  Clay_Color chevronColor;
  Clay_Color color;
  Clay_Color hoverColor;
  size_t defaultAction;
  bool *opened;
  std::function<void()> *onOpen;

  std::vector<ComboButtonAction> actions;
};

void ComboButtonComponent(ComboButtonProps props) {
  CLAY(CLAY_ID("ComboButtonContainer"),
       CLAY_LAYOUT({.layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = props.sizing,
                    .padding = {0, 0},
                    .childGap = 0})) {
    CLAY(CLAY_ID("ComboButton"),
         CLAY_LAYOUT({.layoutDirection = CLAY_LEFT_TO_RIGHT,
                      .sizing = {.width = CLAY_SIZING_GROW(0),
                                 .height = CLAY_SIZING_GROW(0)},
                      .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                         .y = CLAY_ALIGN_Y_CENTER},
                      .padding = {4, 4},
                      .childGap = 8}),
         CLAY_RECTANGLE(
             {.color = Clay_Hovered() ? props.hoverColor : props.color,
              .cornerRadius = 8}),
         Clay_OnHover(ButtonOnClickHandler,
                      reinterpret_cast<intptr_t>(
                          props.actions[props.defaultAction].action))) {
      CLAY_TEXT(props.actions[props.defaultAction].name,
                CLAY_TEXT_CONFIG(ButtonTextConfig));
    }

    CLAY(CLAY_RECTANGLE(
             {.color = Clay_Hovered() ? props.hoverColor : props.color,
              .cornerRadius = 8}),
         CLAY_LAYOUT({.layoutDirection = CLAY_TOP_TO_BOTTOM,
                      .sizing = {.width = CLAY_SIZING_PERCENT(0.2),
                                 .height = CLAY_SIZING_GROW(0)},
                      .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                         .y = CLAY_ALIGN_Y_CENTER},
                      .padding = {4, 4},
                      .childGap = 4}),
         CLAY_BORDER({.left = {.width = 2, .color = props.chevronColor}}),
         Clay_OnHover(ButtonOnClickHandler,
                      reinterpret_cast<intptr_t>(props.onOpen))) {
      CLAY_TEXT(CLAY_STRING(">"), CLAY_TEXT_CONFIG(ButtonTextConfig));
    }
  }

  if (!(*props.opened)) {
    return;
  }

  CLAY(
      CLAY_ID("Options"),
      CLAY_FLOATING(
          {.zIndex = 1,
           .parentId =
               Clay_GetElementId(CLAY_STRING("ComboButtonContainer")).id,
           .attachment = {.element = CLAY_ATTACH_POINT_LEFT_TOP,
                          .parent = CLAY_ATTACH_POINT_LEFT_BOTTOM}}),
      CLAY_LAYOUT({
          .layoutDirection = CLAY_TOP_TO_BOTTOM,
          .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT()},
          .padding = {4, 4},
          .childGap = 4,
      })) {
    for (size_t i = 0; i < props.actions.size(); i++) {
      if (i == props.defaultAction) {
        continue;
      }

      CLAY(CLAY_RECTANGLE(
               {.color = Clay_Hovered() ? props.hoverColor : props.color,
                .cornerRadius = 8}),
           CLAY_LAYOUT({.layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = {.width = CLAY_SIZING_GROW(0),
                                   .height = CLAY_SIZING_FIXED(48)},
                        .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                           .y = CLAY_ALIGN_Y_CENTER},
                        .padding = {4, 4},
                        .childGap = 4}),
           Clay_OnHover(ButtonOnClickHandler,
                        reinterpret_cast<intptr_t>(props.actions[i].action))) {
        CLAY_TEXT(props.actions[i].name, CLAY_TEXT_CONFIG(ButtonTextConfig));
      }
    }
  }
}

/**
 * Does nothing but takes up space, useful for replicating auto-margin
 * behavior
 */
void AutoGrowComponent() {
  CLAY(CLAY_LAYOUT({
      .layoutDirection = CLAY_LEFT_TO_RIGHT,
      .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT()},
      .padding = {0, 0},
  })){};
}
} // namespace

MainInstallerWindow::MainInstallerWindow(std::shared_ptr<Window> window,
                                         std::shared_ptr<Curl> curl,
                                         bool autoUpdate)
    : curl(std::move(curl)), window((window)),
      latestGellyInfo(GetLatestGellyInfo(this->curl)),
      onLaunchClick([this] { HandleOnLaunchClick(); }),
      onInstallClick([this] { HandleOnInstallClick(); }),
      onUninstallClick([this] { HandleOnUninstallClick(); }),
      onComboButtonOpen([this] { HandleOnComboButtonOpen(); }) {
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
    releaseMarkdown =
        helpers::CreateReleaseMarkdown(latestGellyInfo->changelog);
    installButtonString = helpers::CLAY_DYN_STRING(
        std::format("Install {}", latestGellyInfo->version));
    versionString = helpers::CLAY_DYN_STRING(
        std::format("Gelly {}", latestGellyInfo->version));
    changelogHeader =
        helpers::CLAY_DYN_STRING(std::format("{}", latestGellyInfo->version));
  }

  gellyLogo = renderer::LoadTexture(window->GetRenderer(), "gelly-logo.png");
}

void MainInstallerWindow::Render() {
  auto expandLayout =
      Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};

  auto background = Clay_RectangleElementConfig{.color = {15, 15, 15, 255},
                                                .cornerRadius = 8};

  const auto borderColor = Clay_Color{100, 100, 100, 255};

  const std::vector<ComboButtonAction> uninstalledActions = {
      {.name = CLAY_STRING("Install"), .action = &onInstallClick},
  };

  const std::vector<ComboButtonAction> installedOutOfDateActions = {
      {.name = launchButtonString.string, .action = &onLaunchClick},
      {.name = CLAY_STRING("Update"), .action = &onInstallClick},
      {.name = CLAY_STRING("Uninstall"), .action = &onUninstallClick},
  };

  const std::vector<ComboButtonAction> installedUpToDateActions = {
      {.name = launchButtonString.string, .action = &onLaunchClick},
      {.name = CLAY_STRING("Uninstall"), .action = &onUninstallClick},
  };

  const auto actions =
      gellyInstallation.has_value()
          ? gellyInstallation->IsOutdated(latestGellyInfo->version)
                ? installedOutOfDateActions
                : installedUpToDateActions
          : uninstalledActions;

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
       CLAY_LAYOUT({.layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = expandLayout,
                    .padding = {32, 32},
                    .childGap = 32})) {
    CLAY(CLAY_ID("LeftSide"), CLAY_RECTANGLE(background),
         CLAY_LAYOUT({.layoutDirection = CLAY_TOP_TO_BOTTOM,
                      .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                         .y = CLAY_ALIGN_Y_CENTER},
                      .sizing = {.width = CLAY_SIZING_GROW(0),
                                 .height = CLAY_SIZING_GROW(0)},
                      .padding = {16, 16},
                      .childGap = 16})) {
      CLAY(CLAY_ID("GellyLogo"),
           CLAY_LAYOUT({
               .layoutDirection = CLAY_TOP_TO_BOTTOM,
               .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                  .y = CLAY_ALIGN_Y_CENTER},
               .sizing = {.width = CLAY_SIZING_FIXED(512),
                          .height = CLAY_SIZING_FIXED(512)},
           }),
           CLAY_IMAGE(
               {.imageData = gellyLogo, .sourceDimensions = {2000, 1000}})) {}
      ComboButtonComponent({.sizing = {.width = CLAY_SIZING_FIXED(256),
                                       .height = CLAY_SIZING_FIXED(48)},
                            .chevronColor = borderColor,
                            .color = {40, 40, 40, 255},
                            .hoverColor = {50, 50, 50, 255},
                            .opened = &showComboButtonOptions,
                            .onOpen = &onComboButtonOpen,
                            .defaultAction = 0,
                            .actions = actions});
    }
    CLAY(CLAY_ID("RightSide"),
         CLAY_LAYOUT({
             .layoutDirection = CLAY_TOP_TO_BOTTOM,
             .sizing = expandLayout,
             .padding = {16, 16},
             .childGap = 4,
         }),
         CLAY_SCROLL({.vertical = true}),
         CLAY_BORDER({.left = {.width = 2, .color = borderColor}})) {
      CLAY(CLAY_ID("ChangelogHeader"),
           CLAY_LAYOUT({.layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .childAlignment = {.x = CLAY_ALIGN_X_LEFT,
                                           .y = CLAY_ALIGN_Y_CENTER},
                        .sizing = {.width = CLAY_SIZING_GROW(0),
                                   .height = CLAY_SIZING_FIT()},
                        .padding = {0, 4},
                        .childGap = 0}),
           CLAY_BORDER({
               .bottom = {.width = 2, .color = borderColor},
           })) {
        CLAY_TEXT(changelogHeader.string,
                  CLAY_TEXT_CONFIG({
                      .fontId = FONT_ID(FontId::Version48),
                      .fontSize = 48,
                      .textColor = {255, 255, 255, 255},
                  }));
        if (!gellyInstallation ||
            (gellyInstallation &&
             gellyInstallation->IsOutdated(latestGellyInfo->version))) {
          CLAY_TEXT(CLAY_STRING(" (new version available)"),
                    CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID(FontId::Header32),
                        .fontSize = 32,
                        .textColor = {100, 255, 100, 255},
                    }));
        }
      }
      helpers::RenderMarkdown(releaseMarkdown);
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

void MainInstallerWindow::HandleOnComboButtonOpen() {
  showComboButtonOptions = !showComboButtonOptions;
}

} // namespace gelly
