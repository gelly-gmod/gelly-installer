#include "config.hpp"

#include "helpers/parse-version.hpp"
#include "registry.hpp"

#include <array>
#include <windows.h>

namespace gelly {
using helpers::FetchFromGellyRegistry;
using helpers::WriteToGellyRegistry;

optional<std::filesystem::path> Config::GetAppInstallPath() {
  return FetchFromGellyRegistry({.subkey = "InstallPath"});
}

void Config::SetAppInstallPath(const std::filesystem::path &path) {
  WriteToGellyRegistry({.subkey = "InstallPath"}, path.string());
}

optional<std::string> Config::GetAppVersion() {
  return FetchFromGellyRegistry({.subkey = "Version"});
}

void Config::SetAppVersion(const std::string &version) {
  WriteToGellyRegistry({.subkey = "Version"}, version);
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
          {.key = HKEY_CLASSES_ROOT, .parentKey = APP_URI_HANDLER});
      uriHandler.has_value()) {
    return *uriHandler == APP_URI_DEFAULT;
  }
}

} // namespace gelly