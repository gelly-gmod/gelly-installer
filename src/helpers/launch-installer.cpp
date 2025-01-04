#include "launch-installer.hpp"

// clang-format off
#include <windows.h>
#include <shellapi.h>
// clang-format on

namespace gelly::helpers {
namespace {
const auto INSTALLER_EXE = std::string("gelly_installer.exe");
}

void LaunchInstaller(const std::filesystem::path &installerPath) {
  auto exePath = installerPath.string();
  // I don't know why but there are tons of null characters at the end of the
  // string, so we need to remove them
  exePath.erase(std::find(exePath.begin(), exePath.end(), '\0'), exePath.end());
  exePath += "\\" + INSTALLER_EXE + '\0';
  auto workingDir = installerPath.string();
  workingDir.erase(std::find(workingDir.begin(), workingDir.end(), '\0'),
                   workingDir.end());
  workingDir += '\0';

  SHELLEXECUTEINFO info = {};
  info.cbSize = sizeof(SHELLEXECUTEINFO);
  info.fMask = SEE_MASK_NOCLOSEPROCESS;
  info.lpFile = exePath.c_str();
  info.lpDirectory = workingDir.c_str();
  info.lpClass = ".exe";
  info.nShow = SW_SHOW;
  info.lpVerb = "open";
  info.hwnd = nullptr;

  const auto result = ShellExecuteExA(&info);
  if (result == FALSE) {
    MessageBoxW(nullptr, L"Failed to launch installer", L"Error",
                MB_OK | MB_ICONERROR);
    std::exit(1);
  }

  std::exit(0);
}

} // namespace gelly::helpers