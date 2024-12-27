#pragma once
#include <format>
#include <string>
#include <vector>

#undef ERROR

namespace gelly {
enum class LogLevel { INFO, WARNING, ERROR };
struct LogEntry {
  std::string message;
  std::string timestamp;
  LogLevel level;
};

class Log {
public:
  static constexpr auto HEADER = "[gelly-installer log]";
  static constexpr auto LOG_FILE = "gelly-installer.log";

  template <typename... FmtArgs>
  static void AddEntry(const LogLevel &level,
                       const std::format_string<FmtArgs...> &fmt,
                       FmtArgs... args) {
    const auto formattedMessage =
        std::vformat(fmt.get(), std::make_format_args(args...));

    entries.emplace_back(LogEntry{
        .message = formattedMessage, .timestamp = "TODO", .level = level});

#ifdef _DEBUG
    std::printf("%s [%s]: %s\n", HEADER,
                level == LogLevel::INFO      ? "INFO"
                : level == LogLevel::WARNING ? "WARNING"
                                             : "ERROR",
                formattedMessage.c_str());

    std::fflush(stdout);
#endif
  }

  template <typename... FmtArgs>
  static void Info(const std::format_string<FmtArgs...> &fmt, FmtArgs... args) {
    AddEntry(LogLevel::INFO, fmt, args...);
  }

  template <typename... FmtArgs>
  static void Warning(const std::format_string<FmtArgs...> &fmt,
                      FmtArgs... args) {
    AddEntry(LogLevel::WARNING, fmt, args...);
  }

  template <typename... FmtArgs>
  static void Error(const std::format_string<FmtArgs...> &fmt,
                    FmtArgs... args) {
    AddEntry(LogLevel::ERROR, fmt, args...);
  }

  static void SaveToFile();

private:
  static std::vector<LogEntry> entries;

  static std::string FormatEntry(const LogEntry &entry);
};
} // namespace gelly