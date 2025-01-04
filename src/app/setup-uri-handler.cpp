#include "setup-uri-handler.hpp"
#include "app/config.hpp"
#include "registry.hpp"

#include <windows.h>

namespace gelly {
void SetupURIHandler() {
  using namespace helpers;
  if (gelly::Config::IsURIHandlerRegistered()) {
    return;
  }

  const auto installPath = *gelly::Config::GetAppInstallPath();

  // https://learn.microsoft.com/en-us/previous-versions/windows/internet-explorer/ie-developer/platform-apis/aa767914(v=vs.85)?redirectedfrom=MSDN
  WriteToGellyRegistry(HKEY_CLASSES_ROOT, Config::APP_URI_HANDLER, std::nullopt,
                       Config::APP_URI_DEFAULT);
  WriteToGellyRegistry(HKEY_CLASSES_ROOT, Config::APP_URI_HANDLER,
                       "URL Protocol", "");
  WriteToGellyRegistry(HKEY_CLASSES_ROOT, Config::APP_URI_HANDLER,
                       "DefaultIcon", "gelly_installer.exe,1");
  WriteToGellyRegistry(
      HKEY_CLASSES_ROOT,
      std::string(Config::APP_URI_HANDLER) + R"(\shell\open\command)",
      std::nullopt,
      '"' + installPath.string() + R"(\gelly_installer.exe" "%1")");
  // end result: "C:/foo/bar/gelly_installer.exe" "%1"
}
} // namespace gelly