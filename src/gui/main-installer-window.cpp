#include "main-installer-window.hpp"

#include "helpers/find-gmod-directory.hpp"
#include "helpers/find-steam-directory.hpp"
#include "helpers/launch-gmod.hpp"
#include "install/detect-gelly-installation.hpp"
#include "install/get-latest-gelly.hpp"
#include "install/install-gelly.hpp"
#include "install/uninstall-gelly.hpp"

#include <imgui.h>

namespace gelly {
namespace {
constexpr auto HEADER = "Gelly Installer";
}

MainInstallerWindow::MainInstallerWindow(std::shared_ptr<Curl> curl)
    : curl(std::move(curl)), latestGellyInfo(GetLatestGellyInfo(this->curl)) {
  DetectGellyInstallation();
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
  ImGui::Separator();
  ImGui::PopFont();

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

        if (ImGui::Button("Update Gelly")) {
          ImGui::OpenPopup("Outdated Gelly");
        }

        ImGui::SameLine();
      } else {
        ImGui::Text("Gelly is up to date: %s",
                    gellyInstallation->version.c_str());

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
        if (ImGui::Button("Launch")) {
          LaunchGMod();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
      }

      if (ImGui::Button("Uninstall Gelly")) {
        ImGui::OpenPopup("Uninstall Gelly");
      }
    } else {
      ImGui::Text("Gelly is not installed.");
      if (ImGui::Button("Install Gelly")) {
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

  gellyInstallation = gelly::DetectGellyInstallation(*gmodPath);
}

void MainInstallerWindow::CenterPopup() {
  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                          ImGuiCond_Always, ImVec2(0.5f, 0.5f));
}

} // namespace gelly