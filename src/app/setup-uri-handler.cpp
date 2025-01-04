#include "setup-uri-handler.hpp"
#include "app/config.hpp"
#include "registry.hpp"

#include <windows.h>

namespace gelly {
void SetupURIHandler() {
  using namespace helpers;
  if (Config::IsURIHandlerRegistered()) {
    return;
  }

  const auto installPath = *Config::GetAppInstallPath();

  // https://learn.microsoft.com/en-us/previous-versions/windows/internet-explorer/ie-developer/platform-apis/aa767914(v=vs.85)?redirectedfrom=MSDN
  WriteToGellyRegistry(
      {.key = HKEY_CLASSES_ROOT, .parentKey = Config::APP_URI_HANDLER},
      Config::APP_URI_DEFAULT);

  WriteToGellyRegistry({.key = HKEY_CLASSES_ROOT,
                        .parentKey = Config::APP_URI_HANDLER,
                        .subkey = "URL Protocol"},
                       "");

  WriteToGellyRegistry(
      {.key = HKEY_CLASSES_ROOT,
       .parentKey = std::string(Config::APP_URI_HANDLER) + "\\DefaultIcon"},
      "gelly_installer.exe,1");

  WriteToGellyRegistry(
      {.key = HKEY_CLASSES_ROOT,
       .parentKey =
           std::string(Config::APP_URI_HANDLER) + R"(\shell\open\command)"},
      '"' + installPath.string() + R"(\gelly_installer.exe" "%1")");
  // end result: "C:/foo/bar/gelly_installer.exe" "%1"
}
} // namespace gelly