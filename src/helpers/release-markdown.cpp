#include "release-markdown.hpp"

namespace gelly::helpers {
void ReleaseMarkdown::Render() {
  lines = {};
  std::string currentLine;
  int uniqueId = 0;

  for (const auto &c : markdown) {
    if (c == '\n') {
      lines.push_back(currentLine);
      currentLine = "";
    } else {
      currentLine += c;
    }
  }

  lines.push_back(currentLine);

  for (const auto &line : lines) {
    if (line.empty() || line == "\r") {
      continue;
    }

    if (line.front() == '#') {
      MarkdownElementRenderer<MarkdownElement::Header>::Render(ElementData{
          line.c_str() + 3, static_cast<int>(line.size() - 4), uniqueId++});
    } else if (line.front() == '-') {
      MarkdownElementRenderer<MarkdownElement::List>::Render(ElementData{
          line.c_str() + 1, static_cast<int>(line.size() - 2), uniqueId++});
    } else {
      MarkdownElementRenderer<MarkdownElement::Paragraph>::Render(ElementData{
          line.c_str(), static_cast<int>(line.size() - 1), uniqueId++});
    }
  }
}

} // namespace gelly::helpers