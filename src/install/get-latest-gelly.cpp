#include "get-latest-gelly.hpp"

#include "logging/log.hpp"

#include <nlohmann/json.hpp>

namespace gelly {
namespace {
constexpr auto LATEST_RELEASE_URL =
    "https://api.github.com/repos/gelly-gmod/gelly/releases/latest";
}

std::optional<LatestGellyInfo>
GetLatestGellyInfo(const std::shared_ptr<Curl> &curl) {
  const auto response = curl->Get(LATEST_RELEASE_URL);
  if (response->responseCode != 200) {
    Log::Error("Failed to get latest Gelly release info: status code {}",
               response->responseCode);
    return std::nullopt;
  }

  const auto string =
      std::string(response->data->begin(), response->data->end());
  const auto json = nlohmann::json::parse(string);
  const auto version = json["tag_name"].get<std::string>();
  const auto downloadUrl =
      json["assets"][0]["browser_download_url"].get<std::string>();
  const auto changelog = json["body"].get<std::string>();

  return {{
      .version = version,
      .downloadUrl = downloadUrl,
      .changelog = changelog,
  }};
}

} // namespace gelly