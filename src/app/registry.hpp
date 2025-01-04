#pragma once
#include "config.hpp"
#include <string>
#include <windows.h>

namespace gelly::helpers {
const auto REGISTRY_PARENT = std::string("SOFTWARE\\") + Config::APP_NAME;
const auto KEY = HKEY_CURRENT_USER;

std::optional<std::string>
FetchFromGellyRegistry(auto key = KEY,
                       const std::string &parentKey = REGISTRY_PARENT,
                       const std::optional<std::string> &subkey);

void WriteToGellyRegistry(
    auto key = KEY, const std::string &parentKey = REGISTRY_PARENT,
    const std::optional<std::string> &subkey = std::nullopt,
    const std::string &value);
} // namespace gelly::helpers