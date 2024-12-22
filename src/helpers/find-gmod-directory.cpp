#include "find-gmod-directory.hpp"
#include <VDFParser.hpp>
#include <fstream>
#include <ranges>
#include <string>

namespace gelly::helpers {
namespace {
constexpr auto GMOD_APP_ID = "4000";
const std::filesystem::path GMOD_INSTALL_PATH =
    std::filesystem::path("steamapps") / "common" / "GarrysMod";
} // namespace
std::optional<std::filesystem::path>
FindGModDirectory(std::filesystem::path steamPath) {
  const auto libraryFoldersPath =
      steamPath / "steamapps" / "libraryfolders.vdf";
  if (!exists(libraryFoldersPath)) {
    return std::nullopt;
  }

  const auto libraryFoldersStream = std::ifstream(libraryFoldersPath);
  if (!libraryFoldersStream.is_open()) {
    return std::nullopt;
  }

  auto libraryFoldersString = std::stringstream();
  libraryFoldersString << libraryFoldersStream.rdbuf();

  const auto keyValues = VdfParser::fromString(libraryFoldersString.str());
  const auto libraryFoldersBlock = keyValues.getChild("libraryfolders");
  if (!libraryFoldersBlock.has_value()) {
    return std::nullopt;
  }

  const auto folders = libraryFoldersBlock->getChildren();
  if (!folders.has_value()) {
    return std::nullopt;
  }

  for (const auto &entry : folders.value() | std::ranges::views::values) {
    const auto apps = entry.getChild("apps");
    if (!apps.has_value()) {
      continue;
    }

    if (apps->hasChild(GMOD_APP_ID)) {
      const auto libraryPath = entry.getNestedValue({"path"});
      if (!libraryPath.has_value()) {
        continue;
      }

      return std::filesystem::path(libraryPath.value()) / GMOD_INSTALL_PATH;
    }
  }

  return std::nullopt;
}

} // namespace gelly::helpers