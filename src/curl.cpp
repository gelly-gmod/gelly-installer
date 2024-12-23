#include "curl.hpp"

namespace gelly {
Curl::Curl() { curl_global_init(CURL_GLOBAL_ALL); }
Curl::~Curl() { curl_global_cleanup(); }

std::shared_ptr<Response> Curl::Get(const std::string &url) {
  const auto handle = curl_easy_init();
  if (!handle) {
    return nullptr;
  }

  const auto response = std::make_shared<Response>();
  response->data = std::make_shared<Response::Data>();
  curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, response.get());
  curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(handle, CURLOPT_CAINFO, "curl-ca-bundle.crt");
  curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 1L);

  const auto code = curl_easy_perform(handle);
  if (code != CURLE_OK) {
    response->responseCode = code;
  } else {
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response->responseCode);
  }

  return response;
}
size_t Curl::WriteCallback(void *contents, size_t size, size_t nmemb,
                           void *userp) {
  const auto realSize = size * nmemb;
  auto *response = static_cast<Response *>(userp);
  response->data->insert(response->data->end(),
                         static_cast<std::byte *>(contents),
                         static_cast<std::byte *>(contents) + realSize);
  return realSize;
}
} // namespace gelly