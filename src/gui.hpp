#pragma once
#include "gui/main-installer-window.hpp"
#include "window.hpp"

#include <memory>

namespace gelly {
class GUI {
public:
  GUI(std::shared_ptr<Window> window);
  ~GUI();

  bool RunFrame();
  void ProcessSDLEvent(const SDL_Event &ev);

private:
  std::shared_ptr<Window> window;
  MainInstallerWindow mainInstallerWindow;

  void Render();

  void InitializeImGUI() const;
  void ShutdownImGUI();
};
} // namespace gelly
