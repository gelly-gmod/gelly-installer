#include "main-installer-window.hpp"

#include "helpers/find-gmod-directory.hpp"
#include "helpers/find-steam-directory.hpp"
#include "install/detect-gelly-installation.hpp"
#include "install/get-latest-gelly.hpp"

#include <imgui.h>

namespace gelly {
namespace {
constexpr auto HEADER = "Gelly Installer";
}

MainInstallerWindow::MainInstallerWindow(std::shared_ptr<Curl> curl)
    : curl(std::move(curl)) {}

void MainInstallerWindow::Render() {
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::Begin("Installer", &showMainInstallerWindow,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

  ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
  ImGui::SetCursorPosX(
      (ImGui::GetWindowWidth() - ImGui::CalcTextSize(HEADER).x) / 2);
  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

  ImGui::Text(HEADER);
  ImGui::Separator();
  ImGui::PopFont();

  const auto steamPath = helpers::FindSteamDirectory();
  if (steamPath.has_value()) {
    ImGui::Text("Steam found at: %s", steamPath->generic_string().c_str());
    const auto gmodPath = helpers::FindGModDirectory(*steamPath);
    if (gmodPath.has_value()) {
      ImGui::Text("Garry's Mod found at: %s",
                  gmodPath->generic_string().c_str());

      const auto gellyInstallation = DetectGellyInstallation(*gmodPath);
      if (gellyInstallation.has_value()) {
        ImGui::Text("Gelly found at: %s",
                    gellyInstallation->addonPath.generic_string().c_str());
        ImGui::Text("Gelly version: %s", gellyInstallation->version.c_str());

        const auto latestGellyInfo = GetLatestGellyInfo(curl);
        if (latestGellyInfo.has_value()) {
          ImGui::Text("Latest Gelly version: %s",
                      latestGellyInfo->version.c_str());
          ImGui::Text("Download URL: %s", latestGellyInfo->downloadUrl.c_str());
          ImGui::Text("Changelog: %s", latestGellyInfo->changelog.c_str());
        } else {
          ImGui::Text("Failed to fetch latest Gelly version");
        }
      } else {
        ImGui::Text("Gelly not found");
      }
    } else {
      ImGui::Text("Garry's Mod not found");
    }
  } else {
    ImGui::Text("Steam not found");
  }

  ImGui::End();
  ImGui::PopStyleVar();
}

} // namespace gelly