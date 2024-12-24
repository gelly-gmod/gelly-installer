#include "launch-gmod.hpp"
#include <Windows.h>
#include <shellapi.h>

namespace gelly {
namespace {
constexpr auto GMOD_URI = "steam://rungameid/4000";
}

void LaunchGMod() {
  ShellExecuteA(nullptr, "open", GMOD_URI, nullptr, nullptr, SW_SHOWNORMAL);
}
} // namespace gelly