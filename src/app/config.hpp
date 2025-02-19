#pragma once

#include <filesystem>
#include <optional>
#include <version.hpp>

using std::optional;

namespace gelly {
class Config {
public:
  static constexpr auto APP_NAME = "GellyInstaller";
  static constexpr auto APP_VERSION = GELLY_INSTALLER_VERSION;
  static constexpr auto APP_URI_HANDLER = "SOFTWARE\\CLASSES\\gelly-installer";
  static constexpr auto APP_URI_DEFAULT = "URL:Gelly Installer Protocol";

  static optional<std::filesystem::path> GetAppInstallPath();
  static void SetAppInstallPath(const std::filesystem::path &path);

  static optional<std::string> GetAppVersion();
  static void SetAppVersion(const std::string &version);

  static bool IsAppInstalled();
  static bool IsAppUpToDate();
  static bool IsURIHandlerRegistered();
};

} // namespace gelly
