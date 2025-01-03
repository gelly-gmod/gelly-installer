#pragma once
#include "curl.hpp"
#include "gui/main-installer-window.hpp"
#include "window.hpp"

#include <memory>

namespace gelly {
class GUI {
public:
  GUI(std::shared_ptr<Window> window, std::shared_ptr<Curl> curl);
  ~GUI();

  bool RunFrame();
  void ProcessSDLEvent(const SDL_Event &ev);

private:
  std::shared_ptr<Window> window;
  std::shared_ptr<Curl> curl;
  MainInstallerWindow mainInstallerWindow;

  void Render();

  void InitializeImGUI() const;
  void ShutdownImGUI();
};
} // namespace gelly
