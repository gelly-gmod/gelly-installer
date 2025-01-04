#include "find-app-data.hpp"
#include <array>
#include <shlobj_core.h>
#include <windows.h>

namespace gelly::helpers {
std::optional<std::filesystem::path> FindAppData() {
  std::array<TCHAR, MAX_PATH> appDataPath;
  if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_APPDATA, nullptr, 0,
                                appDataPath.data()))) {
    return std::filesystem::path(appDataPath.data());
  }

  return std::nullopt;
}

} // namespace gelly::helpers
