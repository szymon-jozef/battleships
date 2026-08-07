#include "config.hpp"
#include <filesystem>
#include <optional>
#include <spdlog/spdlog.h>

ConfigManager::ConfigManager(const std::optional<const std::filesystem::path> &path)
    : configDir(path.value_or(getConfigPath())) {

  std::filesystem::create_directories(configDir);
  configFile = configDir / std::filesystem::path("config.json");
  spdlog::info("[GUI] settings path is: {}", configFile.string());

  if (!std::filesystem::is_regular_file(configFile)) {
    std::ofstream file(configFile);
    file.close();
    save();
  } else {
    load();
  }
}

std::filesystem::path ConfigManager::getConfigPath() {
#ifdef __linux__
  char *configPathChars = std::getenv("XDG_CONFIG_HOME");
  if (!configPathChars) {
    spdlog::warn("[GUI] could not load $XDG_CONFIG_HOME");
    char *homePathChars = std::getenv("HOME");
    if (!homePathChars) {
      spdlog::error("[GUI] could not get home user directory? Defaulting to cwd...");
      return std::filesystem::path("./.") / "battleships";
    }
    return std::filesystem::path(homePathChars) / ".config/" / "battleships";
  }
  return std::filesystem::path(configPathChars) / "battleships";
#elif _WIN32
  char *configPathChars = std::getenv("LOCALAPPDATA");
  if (!configPathChars) {
    spdlog::warn("[GUI] could not load %LOCALAPPDATA%");
    return std::filesystem::path("./.") / "battleships";
  }
  return std::filesystem::path(configPathChars) / "battleships";
#endif
}

void ConfigManager::save() {
  std::string buffer{};
  glz::error_ctx ec;
  ec = glz::write_json(settings, buffer);

  if (ec) {
    spdlog::error("[GUI] While writing settings to json: {}", ec.custom_error_message);
    return;
  }

  std::ofstream file(configFile);

  if (!file) {
    spdlog::error("[GUI] could not open config file before saving!");
    return;
  }

  file << buffer;
  file.close();

  spdlog::info("[GUI] settings have been saved");
}

void ConfigManager::load() {
  std::ifstream file(configFile);

  if (!file) {
    spdlog::error("[GUI] could not open config file before reading!");
    return;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  glz::error_ctx ec;
  ec = glz::read_json(settings, buffer.str());

  if (ec) {
    spdlog::error("[GUI] While parsing config file: {}", ec.custom_error_message);
    exit(1); // TODO! Do something else here
  }
};

// === Setters, getters and shitters ===

const std::string &ConfigManager::getPlayerName() const {
  return settings.playerName;
}

void ConfigManager::setPlayerName(const std::string &name) {
  settings.playerName = name;
}

const std::string &ConfigManager::getServerUrl() const {
  return settings.serverUrl;
}

void ConfigManager::setServerUrl(const std::string &newUrl) {
  settings.serverUrl = newUrl;
}

uint16_t ConfigManager::getServerPort() const {
  return settings.serverPort;
}

void ConfigManager::setServerPort(const uint16_t newPort) {
  settings.serverPort = newPort;
}

const std::string &ConfigManager::getVolumeLevel() const {
  return settings.volumeLevel;
}

void ConfigManager::setVolumeLevel(const std::string &newVolumeLevel) {
  settings.volumeLevel = newVolumeLevel;
}
