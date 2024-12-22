#include "find-steam-directory.hpp"
#include <Windows.h>
#include <array>

namespace gelly::helpers {
namespace {
constexpr auto STEAM_REGISTRY_KEY = "SOFTWARE\\WOW6432Node\\Valve\\Steam";
constexpr auto STEAM_REGISTRY_VALUE = "InstallPath";

std::optional<std::string> GetRegistryValue(const std::string &key,
                                            const std::string &value) {
  std::array<TCHAR, MAX_PATH> buffer = {};
  DWORD bufferSize = sizeof(buffer);

  if (RegGetValue(HKEY_LOCAL_MACHINE, key.c_str(), value.c_str(), RRF_RT_REG_SZ,
                  nullptr, buffer.data(), &bufferSize) != ERROR_SUCCESS) {
    return std::nullopt;
  }

  return std::string(buffer.data());
}
} // namespace

std::optional<std::filesystem::path> FindSteamDirectory() {
  const auto steamPath =
      GetRegistryValue(STEAM_REGISTRY_KEY, STEAM_REGISTRY_VALUE);

  return steamPath.has_value()
             ? std::optional{std::filesystem::path(*steamPath)}
             : std::nullopt;
}

} // namespace gelly::helpers