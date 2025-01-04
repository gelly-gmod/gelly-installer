#pragma once
#include <optional>
#include <string>
namespace gelly::helpers {
struct Version {
  int major, minor, patch;

  bool operator==(const Version &other) const {
    return major == other.major && minor == other.minor && patch == other.patch;
  }

  bool operator>=(const Version &other) const {
    return major >= other.major && minor >= other.minor && patch >= other.patch;
  }
};

std::optional<Version> ParseVersion(const std::string &version);
} // namespace gelly::helpers
