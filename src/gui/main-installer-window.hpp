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
  bool showOutdatedGellyPopup = true;
  std::string fatalErrorMessage = "";

  void FatalError(const std::string &message);
  void DetectGellyInstallation();

  void CenterPopup();
  void HandleOutdatedGellyPopup();
  void HandleUninstallGellyPopup();
  void HandleFatalErrorPopup();

  bool IsFatalErrorActive() const { return !fatalErrorMessage.empty(); }
};

} // namespace gelly
