#include "detect-gelly-installation.hpp"
#include <Windows.h>

namespace gelly {
namespace {
constexpr auto VERSION_SIGNATURE = 0xfeef04bd;

std::optional<std::string>
ComputeVersionFromBinaryModule(const std::filesystem::path &binaryModulePath) {
  DWORD versionHandle = 0;
  UINT size = 0;
  LPBYTE buffer = nullptr;
  UINT versionSize =
      GetFileVersionInfoSizeW(binaryModulePath.c_str(), &versionHandle);

  if (versionSize == 0) {
    return std::nullopt;
  }

  std::vector<BYTE> versionData(versionSize);
  if (!GetFileVersionInfoW(binaryModulePath.c_str(), versionHandle, versionSize,
                           versionData.data())) {
    return std::nullopt;
  }

  if (!VerQueryValue(versionData.data(), "\\", (LPVOID *)&buffer, &size)) {
    return std::nullopt;
  }

  if (size == 0) {
    return std::nullopt;
  }

  VS_FIXEDFILEINFO *versionInfo = reinterpret_cast<VS_FIXEDFILEINFO *>(buffer);
  if (versionInfo->dwSignature != VERSION_SIGNATURE) {
    return std::nullopt;
  }

  const auto majorVersion = HIWORD(versionInfo->dwFileVersionMS);
  const auto minorVersion = LOWORD(versionInfo->dwFileVersionMS);
  const auto patchVersion = HIWORD(versionInfo->dwFileVersionLS);

  return std::to_string(majorVersion) + "." + std::to_string(minorVersion) +
         "." + std::to_string(patchVersion);
}

} // namespace
std::optional<GellyInstallation>
DetectGellyInstallation(const std::filesystem::path &gmodPath) {
  const auto addonPath = gmodPath / "garrysmod" / "addons" / "gelly";
  const auto binaryModulePath =
      gmodPath / "garrysmod" / "lua" / "bin" / "gmcl_gelly-gmod_win64.dll";

  if (!std::filesystem::exists(addonPath) &&
      !std::filesystem::exists(binaryModulePath)) {
    return std::nullopt;
  }

  bool oldInstallation = !std::filesystem::exists(addonPath / "gelly.gma");

  return {{
      .addonPath = addonPath,
      .binaryModulePath = binaryModulePath,
      .version = ComputeVersionFromBinaryModule(binaryModulePath).value(),
      .oldInstallation = oldInstallation,
  }};
}

} // namespace gelly