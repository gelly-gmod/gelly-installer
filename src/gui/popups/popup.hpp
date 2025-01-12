#pragma once
#include "helpers/clay-dynamic-string.hpp"

#include <functional>
#include <string>

namespace gelly {

class Popup {
public:
  Popup(std::string &&title, std::string &&message,
        std::function<void()> *onClose);
  ~Popup() = default;

  void Render();

private:
  helpers::ClayDynamicString title;
  helpers::ClayDynamicString message;
};

} // namespace gelly
