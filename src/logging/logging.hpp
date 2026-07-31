#pragma once

#include <filesystem>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace battleship::logger {

/// @brief Find the logging path, platform dependent
/// On Linux it will check for:
/// $XDG_STATE_HOME -> $HOME/.local/state -> cwd
///
/// On Windows it will check for:
/// %LOCALAPPDATA% -> cwd
///
/// On everything else it will default to cwd
std::filesystem::path getLoggingPath();

/// @brief Set the default logger and point it at path found by getLoggingPath
/// Sets logging level to env var `SPDLOG_LEVEL`
void setupSpdlogFileLogging();

} // namespace battleship::logger
