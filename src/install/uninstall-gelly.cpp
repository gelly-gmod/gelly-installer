#include "uninstall-gelly.hpp"

namespace gelly {
void UninstallGelly(const GellyInstallation &installation) {
  remove_all(installation.addonPath);
  remove(installation.binaryModulePath);
}
} // namespace gelly
