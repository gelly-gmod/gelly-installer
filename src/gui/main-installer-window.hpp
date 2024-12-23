#pragma once
#include "curl.hpp"

namespace gelly {
class MainInstallerWindow {
public:
  MainInstallerWindow(std::shared_ptr<Curl> curl);
  ~MainInstallerWindow() = default;

  void Render();

private:
  std::shared_ptr<Curl> curl;
  bool showMainInstallerWindow = true;
};

} // namespace gelly
