#include "../main-installer-window.hpp"
#include "install/install-gelly.hpp"

#include <imgui.h>

namespace gelly {
void MainInstallerWindow::HandleOutdatedGellyPopup() {
  CenterPopup();
  if (ImGui::BeginPopupModal("Outdated Gelly", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Gelly is outdated. Do you want to update?");
    if (ImGui::Button("Yes")) {
      InstallGelly(*latestGellyInfo, curl, gellyInstallation);
      DetectGellyInstallation();
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("No")) {
      showOutdatedGellyPopup = false;
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

} // namespace gelly
