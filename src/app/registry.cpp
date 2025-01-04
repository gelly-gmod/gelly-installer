#include "registry.hpp"

#include <array>

namespace gelly::helpers {
std::optional<std::string> FetchFromGellyRegistry(RegistryPath &&registryPath) {
  std::array<char, MAX_PATH> value = {};
  DWORD size = sizeof(value);

  if (RegGetValue(registryPath.key, registryPath.parentKey.c_str(),
                  registryPath.subkey.has_value() ? registryPath.subkey->c_str()
                                                  : nullptr,
                  RRF_RT_REG_SZ, nullptr, value.data(),
                  &size) != ERROR_SUCCESS) {
    return std::nullopt;
  }

  std::string valueStr(value.begin(), value.begin() + (size - 1));
  return valueStr;
}

void WriteToGellyRegistry(RegistryPath &&registryPath,
                          const std::string &value) {
  if (const auto code = RegSetKeyValue(
          registryPath.key, registryPath.parentKey.c_str(),
          registryPath.subkey.has_value() ? registryPath.subkey.value().c_str()
                                          : nullptr,
          RRF_RT_REG_SZ, value.data(), value.size());
      code != ERROR_SUCCESS) {
    throw std::runtime_error("Failed to write to registry: " +
                             std::to_string(code));
  }
}
} // namespace gelly::helpers