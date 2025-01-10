#pragma once
#include "curl.hpp"
#include "gui/main-installer-window.hpp"
#include "window.hpp"

#include <memory>

namespace gelly {
class GUI {
public:
  GUI(std::shared_ptr<Window> window, std::shared_ptr<Curl> curl,
      bool autoUpdate);
  ~GUI();

  bool RunFrame();

private:
  std::shared_ptr<Window> window;
  std::shared_ptr<Curl> curl;
  MainInstallerWindow mainInstallerWindow;
  bool debugEnabled = false;

  void Render();

  void InitializeClay() const;
};
} // namespace gelly
