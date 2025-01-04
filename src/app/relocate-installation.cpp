#include "relocate-installation.hpp"
#include "app/config.hpp"
#include "helpers/find-app-data.hpp"

#include <filesystem>

namespace gelly {
std::filesystem::path RelocateInstallation() {
  const auto cwd = std::filesystem::current_path();
  const auto appData = helpers::FindAppData();
  if (!appData.has_value()) {
    throw std::runtime_error("Failed to find AppData directory");
  }

  const auto newPath = appData.value() / Config::APP_NAME;
  if (!std::filesystem::exists(newPath)) {
    std::filesystem::create_directories(newPath);
  } else {
    std::filesystem::remove_all(newPath);
  }

  std::filesystem::copy(cwd, newPath, std::filesystem::copy_options::recursive);
  return newPath;
}

} // namespace gelly
