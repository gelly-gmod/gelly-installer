#include "registry.hpp"

#include <array>

namespace gelly::helpers {
std::optional<std::string>
FetchFromGellyRegistry(auto key, const std::string &parentKey,
                       const std::optional<std::string> &subkey) {
  std::array<char, MAX_PATH> value = {};
  DWORD size = sizeof(value);

  if (RegGetValue(key, parentKey.c_str(),
                  subkey.has_value() ? subkey->c_str() : nullptr, RRF_RT_REG_SZ,
                  nullptr, value.data(), &size) != ERROR_SUCCESS) {
    return std::nullopt;
  }

  std::string valueStr(value.begin(), value.begin() + (size - 1));
  return valueStr;
}

void WriteToGellyRegistry(auto key, const std::string &parentKey,
                          const std::optional<std::string> &subkey,
                          const std::string &value) {

  if (RegSetKeyValue(
          key, parentKey.c_str(), subkey.has_value() ? subkey.value() : nullptr,
          RRF_RT_REG_SZ, value.data(), value.size()) != ERROR_SUCCESS) {
    throw std::runtime_error("Failed to write to registry");
  }
}
} // namespace gelly::helpers