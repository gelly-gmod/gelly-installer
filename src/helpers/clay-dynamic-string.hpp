#pragma once
#include <clay.h>
#include <vector>
#include <string>

namespace gelly::helpers {
struct ClayDynamicString {
  Clay_String string{};
  std::vector<char> chars{};
};

inline ClayDynamicString CLAY_DYN_STRING(std::string &&str) {
  auto chars = std::vector(str.begin(), str.end());
  return ClayDynamicString{
      .string = {.length = static_cast<int>(chars.size()),
                 .chars = chars.data()},
      .chars = std::move(chars),
  };
}

} // namespace gelly::helpers
