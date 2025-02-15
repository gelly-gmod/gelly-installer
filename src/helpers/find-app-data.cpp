#include "find-app-data.hpp"
#include <array>
// clang-format off
#undef NOUSER
#include <windows.h>
#include <shlobj_core.h>
// clang-format on

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
