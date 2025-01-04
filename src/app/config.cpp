#include "config.hpp"

#include "helpers/parse-version.hpp"
#include "registry.hpp"

#include <array>
#include <windows.h>

namespace gelly {
using helpers::FetchFromGellyRegistry;
using helpers::WriteToGellyRegistry;

optional<std::filesystem::path> Config::GetAppInstallPath() {
  return FetchFromGellyRegistry("InstallPath");
}

void Config::SetAppInstallPath(const std::filesystem::path &path) {
  WriteToGellyRegistry("InstallPath", path.string());
}

optional<std::string> Config::GetAppVersion() {
  return FetchFromGellyRegistry("Version");
}

void Config::SetAppVersion(const std::string &version) {
  WriteToGellyRegistry("Version", version);
}

bool Config::IsAppInstalled() {
  return GetAppInstallPath().has_value() &&
         std::filesystem::exists(*GetAppInstallPath());
}

bool Config::IsAppUpToDate() {
  return IsAppInstalled() && (GetAppVersion().has_value() &&
                              *helpers::ParseVersion(*GetAppVersion()) >=
                                  *helpers::ParseVersion(APP_VERSION));
}

bool Config::IsURIHandlerRegistered() {
  if (const auto uriHandler = FetchFromGellyRegistry(
          HKEY_CLASSES_ROOT, APP_URI_HANDLER, std::nullopt);
      uriHandler.has_value()) {
    return *uriHandler == "1";
  }
}

} // namespace gelly