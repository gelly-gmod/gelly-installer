#pragma once

namespace gelly {
class MainInstallerWindow {
public:
  MainInstallerWindow() = default;
  ~MainInstallerWindow() = default;

  void Render();

private:
  bool showMainInstallerWindow = true;
};

} // namespace gelly
