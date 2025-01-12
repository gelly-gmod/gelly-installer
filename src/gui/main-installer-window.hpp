#pragma once
#include "curl.hpp"
#include "helpers/clay-dynamic-string.hpp"
#include "helpers/release-markdown.hpp"
#include "install/detect-gelly-installation.hpp"
#include "install/get-latest-gelly.hpp"

#include <clay.h>

namespace gelly {
class MainInstallerWindow {
public:
  MainInstallerWindow(std::shared_ptr<Curl> curl, bool autoUpdate);
  ~MainInstallerWindow() = default;

  void Render();

private:
  std::shared_ptr<Curl> curl;
  std::optional<LatestGellyInfo> latestGellyInfo;
  std::optional<GellyInstallation> gellyInstallation;

  bool showMainInstallerWindow = true;
  bool showOutdatedGellyPopup = true;
  std::string fatalErrorMessage = "";
  helpers::ReleaseMarkdown releaseMarkdown;
  helpers::ClayDynamicString launchButtonString = {};

  void FatalError(const std::string &message);
  void DetectGellyInstallation();

  void CenterPopup();
  void HandleOutdatedGellyPopup();
  void HandleUninstallGellyPopup();
  void HandleFatalErrorPopup();

  bool IsFatalErrorActive() const { return !fatalErrorMessage.empty(); }
};

} // namespace gelly
