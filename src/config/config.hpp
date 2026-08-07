#pragma once

#include <cstdint>
#include <glaze/json.hpp>
#include <optional>
#include <string>

struct GameSettings {
  std::string playerName{};
  std::string serverUrl{};
  uint16_t serverPort{};
  std::string volumeLevel{};
};

static_assert(glz::reflectable<GameSettings>);

class ConfigManager {
  std::filesystem::path configDir;
  std::filesystem::path configFile;
  GameSettings settings{"player", "127.0.0.1", 6767, "0.5"};

  /// @brief Scan the system for configuration directory
  /// @return Full path do this program configuration dir
  std::filesystem::path getConfigPath();

public:
  /// @param path Manually set configuration path. Meant mostly for testing
  explicit ConfigManager(const std::optional<const std::filesystem::path> path = std::nullopt);

  void save();
  void load();

  // === Setters, getters and shitters ===

  const std::string &getPlayerName() const;
  void setPlayerName(const std::string &name);

  const std::string &getServerUrl() const;
  void setServerUrl(const std::string &newUrl);

  uint16_t getServerPort() const;
  void setServerPort(const uint16_t newPort);

  const std::string &getVolumeLevel() const;
  void setVolumeLevel(const std::string &newVolumeLevel);
};
