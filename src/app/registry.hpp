#pragma once
#include "config.hpp"
#include <string>
#include <windows.h>

namespace gelly::helpers {
const auto REGISTRY_PARENT = std::string("SOFTWARE\\") + Config::APP_NAME;
const auto KEY = HKEY_CURRENT_USER;

struct RegistryPath {
  HKEY key = KEY;
  std::string parentKey = REGISTRY_PARENT;
  std::optional<std::string> subkey = std::nullopt;
};

std::optional<std::string> FetchFromGellyRegistry(RegistryPath &&registryPath);

void WriteToGellyRegistry(RegistryPath &&registryPath,
                          const std::string &value);

} // namespace gelly::helpers