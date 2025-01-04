#include "detect-elevation.hpp"
#include <windows.h>

namespace gelly::helpers {
bool DetectElevatedPrivileges() {
  HANDLE token;
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
    return false;
  }

  TOKEN_ELEVATION elevation;
  DWORD size;
  if (!GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation),
                           &size)) {
    CloseHandle(token);
    return false;
  }

  return elevation.TokenIsElevated;
}
} // namespace gelly::helpers
