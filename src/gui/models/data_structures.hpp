#pragma once
#include <atomic>
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
      save();
      file.close();
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

    std::string line, type, value;
    size_t delimeterPos;

    while (std::getline(file, line)) {
      delimeterPos = line.find('=');
      type = line.substr(0, delimeterPos);
      value = line.substr(delimeterPos + 1);

      if (type == "playerName") {
        playerName = value;
      } else if (type == "serverUrl") {
        serverUrl = value;
      } else if (type == "serverPort") {
        serverPort = std::atoi(value.c_str());
      } else {
        spdlog::warn("[GUI] unrecognize entry in the settings: {} - {}", type, value);
        continue;
      }
      spdlog::info("[GUI] setting have been loaded: {} - {}", type.c_str(), value.c_str());
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
