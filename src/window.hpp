#pragma once
#include <clay.h>

#undef CreateWindow

namespace gelly {
class Window {
public:
  static constexpr auto FONT_ID_BODY_16 = 0;

  Window();
  ~Window();

  template <typename FrameHandler>
  void RunEventLoop(FrameHandler frameHandler) {
    while (!ShouldClose()) {
      if (reinitializeClay) {
        ReinitializeClay();
      }

      if (!frameHandler()) {
        break;
      }
    }
  }

  void RenderCommands(const Clay_RenderCommandArray &array);

private:
  static bool reinitializeClay;
  static void HandleClayErrors(Clay_ErrorData error);

  bool ShouldClose() const;
  void ReinitializeClay();
};

} // namespace gelly
