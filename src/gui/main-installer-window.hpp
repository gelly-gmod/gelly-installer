#pragma once
#include "curl.hpp"
#include "install/detect-gelly-installation.hpp"
#include "install/get-latest-gelly.hpp"

namespace gelly {
class MainInstallerWindow {
public:
  MainInstallerWindow(std::shared_ptr<Curl> curl);
  ~MainInstallerWindow() = default;

  void Render();

private:
  std::shared_ptr<Curl> curl;
  std::optional<LatestGellyInfo> latestGellyInfo;
  std::optional<GellyInstallation> gellyInstallation;
  bool showMainInstallerWindow = true;

  void DetectGellyInstallation();
};

} // namespace gelly
