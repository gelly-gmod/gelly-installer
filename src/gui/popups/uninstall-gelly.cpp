#include "install/uninstall-gelly.hpp"

#include "../main-installer-window.hpp"
#include "install/install-gelly.hpp"

#include <imgui.h>

namespace gelly {
void MainInstallerWindow::HandleUninstallGellyPopup() {
  CenterPopup();
  if (ImGui::BeginPopupModal("Uninstall Gelly", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Do you want to uninstall Gelly?");
    if (ImGui::Button("Yes")) {
      UninstallGelly(*gellyInstallation);
      DetectGellyInstallation();
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("No")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}
} // namespace gelly