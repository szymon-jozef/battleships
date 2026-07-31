#include "logging.hpp"
#include <filesystem>
#include <iostream>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace battleship::logger {

std::filesystem::path getLoggingPath() {
#ifdef __linux__
  constexpr auto appName = "battleships";
  char *statePath = std::getenv("XDG_STATE_HOME");

  if (statePath) {
    return std::filesystem::path(statePath) / appName / "logs";
  }

  char *homePath = std::getenv("HOME");

  if (homePath) {
    return std::filesystem::path(homePath) / ".local/state" / appName / "logs";
  }

#elif _WIN32
  constexpr auto appName = "battleships";
  if (char *localAppDataPath = std::getenv("LOCALAPPDATA")) {
    return std::filesystem::path(localAppDataPath) / appName / "logs";
  }

#endif
  return std::filesystem::current_path() / "logs";
}

void setupSpdlogFileLogging() {
  const std::filesystem::path loggingPath = getLoggingPath();
  const std::filesystem::path loggingFilePath = loggingPath / "log.txt";

  spdlog::info("Logs will be redirected to: {}", loggingFilePath.string());

  std::filesystem::create_directories(loggingPath);

  try {
    const auto logger = spdlog::basic_logger_mt("Logs", loggingFilePath.string());
    spdlog::set_default_logger(logger);

  } catch (const spdlog::spdlog_ex &ex) {
    std::cout << "Log init failed: " << ex.what() << std::endl;
  }

  spdlog::cfg::load_env_levels();
}

} // namespace battleship::logger
