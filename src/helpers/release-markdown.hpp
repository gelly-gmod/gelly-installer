#pragma once
#include "window.hpp"

#include <clay.h>
#include <string>
#include <vector>

namespace gelly::helpers {
enum class MarkdownElement {
  Header,
  Paragraph,
  List,
};

struct ElementData {
  const char *data;
  int length;
  int uniqueId;
};

template <auto T> struct MarkdownElementRenderer {
  static void Render(const ElementData &data);
};

template <> struct MarkdownElementRenderer<MarkdownElement::Header> {
  static void Render(const ElementData &data) {
    Clay_String string = {.length = data.length, .chars = data.data};
    CLAY(CLAY_IDI("MDElement", data.uniqueId),
         CLAY_TEXT(string, CLAY_TEXT_CONFIG({
                               .fontId = FONT_ID(FontId::Header32),
                               .fontSize = 32,
                               .textColor = {255, 255, 255, 255},
                           })));
  }
};

template <> struct MarkdownElementRenderer<MarkdownElement::Paragraph> {
  static void Render(const ElementData &data) {
    Clay_String string = {.length = data.length, .chars = data.data};
    CLAY(CLAY_IDI("MDElement", data.uniqueId),
         CLAY_TEXT(string, CLAY_TEXT_CONFIG({
                               .fontId = FONT_ID(FontId::Body16),
                               .fontSize = 16,
                               .textColor = {255, 255, 255, 255},
                           })));
  }
};

template <> struct MarkdownElementRenderer<MarkdownElement::List> {
  static void Render(const ElementData &data) {
    Clay_String string = {.length = data.length, .chars = data.data};
    CLAY(
        CLAY_IDI("MDElement", data.uniqueId),
        CLAY_LAYOUT({
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = {.width = CLAY_SIZING_FIT(), .height = CLAY_SIZING_FIT()},
            .padding = {24, 0},
        }),
        CLAY_TEXT(string, CLAY_TEXT_CONFIG({
                              .fontId = FONT_ID(FontId::Body16),
                              .fontSize = 16,
                              .textColor = {255, 255, 255, 255},
                          })));
  }
};

class ReleaseMarkdown {
public:
  ReleaseMarkdown(const std::string &markdown) : markdown(markdown) {}
  ReleaseMarkdown() : markdown("") {}
  ~ReleaseMarkdown() = default;

  /**
   * Renders into the current Clay context.
   */
  void Render();

private:
  std::string markdown;
  std::vector<std::string> lines;
};
} // namespace gelly::helpers
