#include "config.hpp"
#include <filesystem>
#include <optional>
#include <raylib.h>
#include <spdlog/spdlog.h>

SettingsManager::SettingsManager(const std::optional<const std::filesystem::path> path)
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

std::filesystem::path SettingsManager::getConfigPath() {
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

void SettingsManager::save() {
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

  SetMasterVolume(std::stof(settings.volumeLevel));
  spdlog::info("[GUI] master volume set at: {}", GetMasterVolume());
}

void SettingsManager::load() {
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

const std::string &SettingsManager::getPlayerName() const {
  return settings.playerName;
}

void SettingsManager::setPlayerName(const std::string &name) {
  settings.playerName = name;
}

const std::string &SettingsManager::getServerUrl() const {
  return settings.serverUrl;
}

void SettingsManager::setServerUrl(const std::string &newUrl) {
  settings.serverUrl = newUrl;
}

uint16_t SettingsManager::getServerPort() const {
  return settings.serverPort;
}

void SettingsManager::setServerPort(const uint16_t newPort) {
  settings.serverPort = newPort;
}

const std::string &SettingsManager::getVolumeLevel() const {
  return settings.volumeLevel;
}

void SettingsManager::setVolumeLevel(const std::string &newVolumeLevel) {
  settings.volumeLevel = newVolumeLevel;
}
