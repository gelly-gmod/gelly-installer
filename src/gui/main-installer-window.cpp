#include "main-installer-window.hpp"

#include <imgui.h>

namespace gelly {
void MainInstallerWindow::Render() {
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::Begin("Installer", &showMainInstallerWindow,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

  ImGui::Text("Hello, world!");

  ImGui::End();
  ImGui::PopStyleVar();
}

} // namespace gelly