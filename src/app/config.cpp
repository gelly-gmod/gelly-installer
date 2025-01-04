#include "config.hpp"

#include "helpers/parse-version.hpp"

#include <windows.h>

namespace gelly {
namespace {
auto REGISTRY_PARENT = std::string("SOFTWARE\\") + Config::APP_NAME;
auto KEY = HKEY_CURRENT_USER;

std::optional<std::string> FetchFromGellyRegistry(const std::string &subkey) {
  std::string value(MAX_PATH, '\0');
  DWORD size = value.size();

  const auto path = REGISTRY_PARENT;
  if (RegGetValue(KEY, path.c_str(), subkey.c_str(), RRF_RT_REG_SZ, nullptr,
                  value.data(), &size) != ERROR_SUCCESS) {
    return std::nullopt;
  }

  return value;
}

void WriteToGellyRegistry(const std::string &subkey, const std::string &value) {
  if (RegSetKeyValue(KEY, REGISTRY_PARENT.c_str(), subkey.c_str(),
                     RRF_RT_REG_SZ, value.data(),
                     value.size()) != ERROR_SUCCESS) {
    throw std::runtime_error("Failed to write to registry");
  }
}
} // namespace

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
} // namespace gelly