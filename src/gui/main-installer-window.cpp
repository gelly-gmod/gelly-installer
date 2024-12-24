#include "main-installer-window.hpp"

#include "helpers/find-gmod-directory.hpp"
#include "helpers/find-steam-directory.hpp"
#include "install/detect-gelly-installation.hpp"
#include "install/get-latest-gelly.hpp"
#include "install/install-gelly.hpp"

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
  if (!latestGellyInfo.has_value()) {
    ImGui::OpenPopup("Fatal Error");
    if (ImGui::BeginPopupModal("Fatal Error", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::Text("Failed to fetch latest Gelly version. Try restarting the "
                  "installer later.");
      if (ImGui::Button("OK")) {
        ImGui::CloseCurrentPopup();
        std::exit(1);
      }
      ImGui::EndPopup();
    }
    return;
  }

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

  if (ImGui::Button("Upgrade")) {
    InstallGelly(*latestGellyInfo, curl, gellyInstallation);
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

} // namespace gelly