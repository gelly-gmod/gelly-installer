#include "../main-installer-window.hpp"
#include <imgui.h>

namespace gelly {
void MainInstallerWindow::FatalError(const std::string &message) {
  fatalErrorMessage = message;
  ImGui::OpenPopup("Fatal Error");
}

void MainInstallerWindow::HandleFatalErrorPopup() {
  CenterPopup();
  if (ImGui::BeginPopupModal("Fatal Error", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text(fatalErrorMessage.c_str());
    if (ImGui::Button("OK")) {
      ImGui::CloseCurrentPopup();
      std::exit(1);
    }
    ImGui::EndPopup();
  }
}

} // namespace gelly
