#include "gui.hpp"

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <imgui.h>

namespace gelly {
GUI::GUI(std::shared_ptr<Window> window, std::shared_ptr<Curl> curl)
    : window(std::move(window)), curl(std::move(curl)), mainInstallerWindow(curl) {
  InitializeImGUI();
}

GUI::~GUI() { ShutdownImGUI(); }

bool GUI::RunFrame() {
  ImGui_ImplSDLRenderer2_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  mainInstallerWindow.Render();

  Render();

  return true;
}

void GUI::ProcessSDLEvent(const SDL_Event &ev) {
  ImGui_ImplSDL2_ProcessEvent(&ev);
}

void GUI::Render() {
  const auto io = ImGui::GetIO();
  ImGui::Render();
  SDL_RenderSetScale(window->GetRenderer(), io.DisplayFramebufferScale.x,
                     io.DisplayFramebufferScale.y);
  SDL_SetRenderDrawColor(window->GetRenderer(), 0, 0, 0, 255);
  SDL_RenderClear(window->GetRenderer());
  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(),
                                        window->GetRenderer());
  SDL_RenderPresent(window->GetRenderer());
}

void GUI::InitializeImGUI() const {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  auto &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 16.0f);
  io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 72.0f);
  io.FontDefault = io.Fonts->Fonts[0];

  ImGui_ImplSDL2_InitForSDLRenderer(window->GetWindow(), window->GetRenderer());
  ImGui_ImplSDLRenderer2_Init(window->GetRenderer());
}

void GUI::ShutdownImGUI() {
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}

} // namespace gelly