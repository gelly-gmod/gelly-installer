#include "parse-version.hpp"

namespace gelly::helpers {
std::optional<Version> ParseVersion(const std::string &version) {
  Version parsed;
  const auto count = sscanf_s(version.c_str(), "%d.%d.%d", &parsed.major,
                              &parsed.minor, &parsed.patch);
  if (count != 3) {
    return std::nullopt;
  }

  return parsed;
}
} // namespace gelly::helpers
