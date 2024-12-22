#pragma once
#include <filesystem>
#include <optional>

namespace gelly::helpers {
std::optional<std::filesystem::path>
FindGModDirectory(std::filesystem::path steamPath);
}