#include "main-installer-window.hpp"

#include "helpers/find-gmod-directory.hpp"
#include "helpers/find-steam-directory.hpp"
#include "helpers/launch-gmod.hpp"
#include "install/detect-gelly-installation.hpp"
#include "install/get-latest-gelly.hpp"
#include "install/install-gelly.hpp"
#include "install/uninstall-gelly.hpp"
#include "logging/log.hpp"

#include <imgui.h>

namespace gelly {
namespace {
constexpr auto HEADER = "Gelly Installer";
}

MainInstallerWindow::MainInstallerWindow(std::shared_ptr<Curl> curl)
    : curl(std::move(curl)), latestGellyInfo(GetLatestGellyInfo(this->curl)) {
  Log::Info("Latest Gelly version: {}",
            latestGellyInfo.has_value() ? latestGellyInfo->version : "N/A");

  DetectGellyInstallation();
  Log::Info("Gelly installation detected: {}",
            gellyInstallation.has_value() ? "yes" : "no");
  Log::Info("Gelly version: {}",
            gellyInstallation.has_value() ? gellyInstallation->version : "N/A");
}

void MainInstallerWindow::Render() {
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::Begin("Installer", &showMainInstallerWindow,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

  if (!latestGellyInfo.has_value()) {
    FatalError("Failed to get the latest Gelly info. Try again later. The "
               "installer will now exit.");
  }

  HandleFatalErrorPopup();
  HandleOutdatedGellyPopup();
  HandleUninstallGellyPopup();

  if (IsFatalErrorActive()) {
    ImGui::End();
    ImGui::PopStyleVar();
    return;
  }

  ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
  ImGui::SetCursorPosX(
      (ImGui::GetWindowWidth() - ImGui::CalcTextSize(HEADER).x) / 2);
  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

  ImGui::Text(HEADER);

  if (gellyInstallation.has_value()) {
    ImGui::SetCursorPosX(
        (ImGui::GetWindowWidth() -
         ImGui::CalcTextSize(gellyInstallation->version.c_str()).x) /
        2);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
    ImGui::TextColored(ImVec4(0.1f, 0.8f, 0.1f, 1.0f), "%s",
                       gellyInstallation->version.c_str());
  }

  ImGui::Separator();
  ImGui::PopFont();

  const auto buttonSize = ImVec2(ImGui::GetWindowWidth() - 20, 40);
  // dock buttons to the bottom of the window
  ImGui::SetCursorPosY(ImGui::GetWindowHeight() - buttonSize.y * 2.5);

  if (gellyInstallation.has_value() &&
      gellyInstallation->IsOutdated(latestGellyInfo->version) &&
      showOutdatedGellyPopup) {
    ImGui::OpenPopup("Outdated Gelly");
  } else {
    if (gellyInstallation.has_value()) {
      if (gellyInstallation->IsOutdated(latestGellyInfo->version)) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                           "Gelly is out of date: %s",
                           gellyInstallation->version.c_str());

        if (ImGui::Button("Update Gelly", buttonSize)) {
          ImGui::OpenPopup("Outdated Gelly");
        }
      } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
        if (ImGui::Button("Launch", buttonSize)) {
          LaunchGMod();
        }
        ImGui::PopStyleColor();
      }

      if (ImGui::Button("Uninstall Gelly", buttonSize)) {
        ImGui::OpenPopup("Uninstall Gelly");
      }
    } else {
      if (ImGui::Button("Install Gelly", buttonSize)) {
        try {
          InstallGelly(*latestGellyInfo, curl, std::nullopt);
          DetectGellyInstallation();
        } catch (const std::exception &e) {
          FatalError(e.what());
        }
      }
    }
  }

  ImGui::End();
  ImGui::PopStyleVar();
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

void MainInstallerWindow::CenterPopup() {
  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                          ImGuiCond_Always, ImVec2(0.5f, 0.5f));
}

} // namespace gelly