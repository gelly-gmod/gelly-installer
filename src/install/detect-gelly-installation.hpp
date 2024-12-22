#pragma once
#include <filesystem>
#include <optional>

namespace gelly {
struct GellyInstallation {
  std::filesystem::path addonPath;
  std::filesystem::path binaryModulePath;
  std::string version;
  bool oldInstallation;

  bool IsOutdated(const std::string &latestVersion) const {
    return version != latestVersion;
  }
};

std::optional<GellyInstallation>
DetectGellyInstallation(const std::filesystem::path &gmodPath);
} // namespace gelly