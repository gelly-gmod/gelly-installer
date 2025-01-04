#pragma once

#include <filesystem>
#include <optional>
using std::optional;

namespace gelly {
class Config {
public:
  static constexpr auto APP_NAME = "GellyInstaller";
  // TODO: Fetch version from changelog
  static constexpr auto APP_VERSION = "1.0.0";

  static optional<std::filesystem::path> GetAppInstallPath();
  static void SetAppInstallPath(const std::filesystem::path &path);

  static optional<std::string> GetAppVersion();
  static void SetAppVersion(const std::string &version);

  static bool IsAppInstalled();
  static bool IsAppUpToDate();
};

} // namespace gelly
