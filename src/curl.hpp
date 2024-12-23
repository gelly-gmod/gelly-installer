#pragma once
#include <curl/curl.h>
#include <memory>
#include <string>
#include <vector>

namespace gelly {
struct Response {
  using Data = std::vector<std::byte>;
  std::shared_ptr<Data> data;
  long responseCode{};
};

struct CurlRequestInfo {
  std::shared_ptr<Response> response;
};

class Curl {
public:
  Curl();
  ~Curl();

  static std::shared_ptr<Response> Get(const std::string &url);
  static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                              void *userp);
};
} // namespace gelly
