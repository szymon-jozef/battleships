#pragma once
#include <atomic>
#include <charconv>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <string>

namespace battleship {
namespace gui {

enum class GuiState {
  MAIN_MENU,
  MODE_SELECTION,

  GAME,
  JOIN_SERVER,
  GAME_FINISH,

  SETTINGS,
  QUIT,
};

class GameSettings {
  std::filesystem::path configPath;
  std::filesystem::path configFilePath;

  void loadPaths() {
#ifdef __linux__
    char *configPathChars = std::getenv("XDG_CONFIG_HOME");
    if (!configPathChars) {
      spdlog::warn("[GUI] could not load $XDG_CONFIG_HOME");
      char *homePathChars = std::getenv("HOME");
      if (!homePathChars) {
        spdlog::error("[GUI] could not get home user directory? Defaulting to cwd...");
        configPath = std::filesystem::path("./.");
        return;
      }
      configPath = std::filesystem::path(homePathChars) / ".config/";
      return;
    }
    configPath = std::filesystem::path(configPathChars);
#elif _WIN32
    char *configPathChars = std::getenv("LOCALAPPDATA");
    if (!configPathChars) {
      spdlog::warn("[GUI] could not load %LOCALAPPDATA%");
      configPath = std::filesystem::path("./.");
      return;
    }
    configPath = std::filesystem::path(configPathChars);
#endif
  }

public:
  std::string playerName;
  std::string serverUrl;
  uint16_t serverPort;

  GameSettings() {
    loadPaths();
    configPath /= "battleships";

    std::filesystem::create_directories(configPath);

    configFilePath = configPath / std::filesystem::path("config.cfg");
    spdlog::info("[GUI] settings path is: {}", configFilePath.c_str());

    if (!std::filesystem::is_regular_file(configFilePath)) {
      std::ofstream file(configFilePath);
      playerName = "player";
      serverUrl = "127.0.0.1";
      serverPort = 6767;
      file.close();
      save();
    }
  }

  void save() {
    std::ofstream file(configFilePath);

    if (!file) {
      spdlog::error("[GUI] could not open config file before saving!");
      return;
    }

    file << "playerName=" << playerName << '\n';
    file << "serverUrl=" << serverUrl << '\n';
    file << "serverPort=" << serverPort << '\n';

    file.close();
    spdlog::info("[GUI] settings have been saved");
  }
  void load() {
    std::ifstream file(configFilePath);

    if (!file) {
      spdlog::error("[GUI] could not open config file before reading!");
      return;
    }

    std::string line;
    std::string_view line_view, type, value;
    size_t delimeterPos;

    while (std::getline(file, line)) {
      line_view = {line};
      delimeterPos = line_view.find('=');
      if (delimeterPos == 0 || delimeterPos == std::string::npos) {
        continue;
      }

      type = line_view.substr(0, delimeterPos);
      value = line_view.substr(delimeterPos + 1);

      if (type == "playerName") {
        playerName = value;
      } else if (type == "serverUrl") {
        serverUrl = value;
      } else if (type == "serverPort") {
        auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), serverPort);

        if (ec == std::errc::invalid_argument) {
          spdlog::error("[GUI] server port was set as something that's not a number!");
          serverPort = 6767;
        } else if (ec == std::errc::result_out_of_range) {
          spdlog::error("[GUI] server port was set as value out of range!");
          serverPort = 6767;
        }

      } else {
        spdlog::warn("[GUI] unrecognize entry in the settings: {} - {}", type, value);
        continue;
      }
      spdlog::info("[GUI] setting have been loaded: {} - {}", type, value);
    }
  };
};

struct GameContext {
  GameSettings settings;
  std::string loserName;
  bool isWon;
  std::atomic<GuiState> guiState = GuiState::MAIN_MENU;
  enum class GameMode { HOSTING, JOINING };
  GameMode currentGameMode = GameMode::HOSTING;
};

} // namespace gui
} // namespace battleship
