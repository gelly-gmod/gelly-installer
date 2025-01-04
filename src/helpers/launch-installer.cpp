#include "launch-installer.hpp"

// clang-format off
#include "logging/log.hpp"


#include <windows.h>
#include <shellapi.h>
// clang-format on

namespace gelly::helpers {
namespace {
const auto INSTALLER_EXE = std::string("gelly_installer.exe");
}

void LaunchInstaller(const std::filesystem::path &installerPath,
                     bool askForElevation, int argc, char *argv[]) {
  gelly::Log::SaveToFile();
  auto exePath = installerPath.string() + "\\" + INSTALLER_EXE;
  auto workingDir = installerPath.string();

  SHELLEXECUTEINFO info = {};
  info.cbSize = sizeof(SHELLEXECUTEINFO);
  info.fMask = SEE_MASK_NOCLOSEPROCESS;
  info.lpFile = exePath.c_str();
  info.lpDirectory = workingDir.c_str();
  info.lpClass = ".exe";
  info.nShow = SW_SHOW;
  info.lpVerb = "open";
  info.hwnd = nullptr;

  std::string args;
  if (argc > 1) {
    for (auto i = 1; i < argc; ++i) {
      args += argv[i];
      args += " ";
    }

    info.lpParameters = args.c_str();
  }

  if (askForElevation) {
    info.lpVerb = "runas";
  }

  const auto result = ShellExecuteExA(&info);
  if (result == FALSE) {
    MessageBoxW(nullptr, L"Failed to launch installer", L"Error",
                MB_OK | MB_ICONERROR);
    std::exit(1);
  }

  std::exit(0);
}

} // namespace gelly::helpers