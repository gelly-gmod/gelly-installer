#include "log.hpp"

#include <cstdio>
#include <format>
#include <fstream>

namespace gelly {
namespace {
constexpr auto LOG_FORMAT = "[{}] {}\n";

std::string FormatLogLevel(LogLevel level) {
  switch (level) {
  case LogLevel::INFO:
    return "INFO";
  case LogLevel::WARNING:
    return "WARNING";
  case LogLevel::ERROR:
    return "ERROR";
  default:
    return "UNKNOWN";
  }
}

} // namespace

std::vector<LogEntry> Log::entries = {};

std::string Log::FormatEntry(const LogEntry &entry) {
  const auto levelStr = FormatLogLevel(entry.level);

  return std::format(LOG_FORMAT, levelStr, entry.message);
}

void Log::SaveToFile() {
  auto file = std::ofstream(LOG_FILE);
  file << HEADER;
  file << '\n';

  for (const auto &entry : entries) {
    file << FormatEntry(entry);
  }

  file.close();
}

} // namespace gelly
