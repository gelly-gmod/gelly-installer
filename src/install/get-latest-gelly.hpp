#pragma once
#include "curl.hpp"

#include <optional>
#include <string>

namespace gelly {
struct LatestGellyInfo {
  std::string version;
  std::string downloadUrl;
  std::string changelog;
};

std::optional<LatestGellyInfo>
GetLatestGellyInfo(const std::shared_ptr<Curl> &curl);
} // namespace gelly