#pragma once
#include <filesystem>

namespace gelly::helpers {
[[noreturn]] void LaunchInstaller(const std::filesystem::path &installerPath,
                                  bool askForElevation = false, int argc = 0,
                                  char *argv[] = nullptr);
}
