#include "release-markdown.hpp"

#include <optional>
#include <queue>

namespace gelly::helpers {
Tape CreateTapeFromMarkdown(const std::string &markdown) {
  Tape tape = {};
  std::string line;

  for (const auto &character : markdown) {
    if (character == '\n') {
      tape.lineStorage.push_back(line);
      line.clear();
    } else {
      line += character;
    }
  }

  tape.lineStorage.push_back(line);

  // create the pointer queue
  for (auto &line : tape.lineStorage) {
    tape.lines.push(&line);
  }

  return tape;
}

constexpr auto HEADER_TITLE_OFFSET = sizeof("### ") - 1;

std::optional<Section> TakeSectionFromTape(Tape &tape) {
  bool initialized = false;
  Section section = {};

  while (auto line = tape.Peek()) {
    if (!line) {
      break;
    }

    if (line->starts_with("#") && !initialized) {
      section.title = {.length =
                           static_cast<int>(line->size() - HEADER_TITLE_OFFSET),
                       .chars = line->data() + HEADER_TITLE_OFFSET};
      initialized = true;
    } else if (line->starts_with("#") && initialized) {
      break;
    } else {
      section.lines.push_back(
          {.length = static_cast<int>(line->size()), .chars = line->data()});
    }

    tape.Take();
  }

  if (!initialized) {
    return std::nullopt;
  }

  return section;
}

ReleaseMarkdown CreateReleaseMarkdown(const std::string &markdown) {
  ReleaseMarkdown releaseMarkdown = {.tape = CreateTapeFromMarkdown(markdown)};

  while (auto section = TakeSectionFromTape(releaseMarkdown.tape)) {
    releaseMarkdown.sections.push_back(*section);
  }

  return releaseMarkdown;
}

void RenderMarkdown(const ReleaseMarkdown &markdown) {
  int uniqueId = 0;
  for (const auto &section : markdown.sections) {
    CLAY(
        CLAY_IDI("MDElement", uniqueId),
        CLAY_LAYOUT({
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_FIT(), .height = CLAY_SIZING_FIT()},
            .padding = {4, 4},
            .childGap = 4,
        })) {
      CLAY_TEXT(section.title, CLAY_TEXT_CONFIG({
                                   .fontId = FONT_ID(FontId::Header32),
                                   .fontSize = 32,
                                   .textColor = {255, 255, 255, 255},
                               }));
      for (const auto &line : section.lines) {
        CLAY_TEXT(line, CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID(FontId::Body16),
                            .fontSize = 16,
                            .textColor = {255, 255, 255, 255},
                        }));
      }
    }
  }
}

} // namespace gelly::helpers