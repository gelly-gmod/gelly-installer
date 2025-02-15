#pragma once
#include "window.hpp"

#include <clay.h>
#include <queue>
#include <string>
#include <vector>

namespace gelly::helpers {
struct Section {
  Clay_String title;
  std::vector<Clay_String> lines;
};

struct Tape {
  std::vector<std::string> lineStorage;
  std::queue<std::string *> lines;

  std::string *Peek() {
    if (lines.empty()) {
      return nullptr;
    }

    return lines.front();
  }

  std::string *Take() {
    if (lines.empty()) {
      return nullptr;
    }

    auto line = lines.front();
    lines.pop();
    return line;
  }
};

struct ReleaseMarkdown {
  std::vector<Section> sections;
  Tape tape;
};

ReleaseMarkdown CreateReleaseMarkdown(const std::string &markdown);
void RenderMarkdown(const ReleaseMarkdown &markdown);
} // namespace gelly::helpers
