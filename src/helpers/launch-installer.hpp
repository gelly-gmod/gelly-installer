#pragma once
#include <filesystem>

namespace gelly::helpers {
[[noreturn]] void LaunchInstaller(const std::filesystem::path &installerPath);
}
