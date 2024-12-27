#include "uninstall-gelly.hpp"

#include "logging/log.hpp"

namespace gelly {
void UninstallGelly(const GellyInstallation &installation) {
  remove_all(installation.addonPath);
  remove(installation.binaryModulePath);
  Log::Info("Uninstalled Gelly at {}", installation.addonPath.string());
}
} // namespace gelly
