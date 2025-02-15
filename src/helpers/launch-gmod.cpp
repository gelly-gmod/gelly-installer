#include "launch-gmod.hpp"
// clang-format off
#undef NOUSER
#include <windows.h>
#include <shellapi.h>
// clang-format on

namespace gelly {
namespace {
constexpr auto GMOD_URI = "steam://rungameid/4000";
}

void LaunchGMod() {
  ShellExecuteA(nullptr, "open", GMOD_URI, nullptr, nullptr, SW_SHOWNORMAL);
}
} // namespace gelly