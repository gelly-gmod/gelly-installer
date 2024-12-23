#pragma once
#include <string>

namespace gelly {
struct LatestGellyInfo {
  std::string version;
  std::string downloadUrl;
  std::string changelog;
};

LatestGellyInfo GetLatestGellyInfo();
} // namespace gelly