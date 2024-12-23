#pragma once
#include "detect-gelly-installation.hpp"
#include "get-latest-gelly.hpp"

namespace gelly {
void InstallGelly(const LatestGellyInfo &info,
                  const std::shared_ptr<Curl> &curl,
                  std::optional<GellyInstallation> priorInstallation);
}
