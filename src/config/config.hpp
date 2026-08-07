#pragma once

#include <glaze/json.hpp>
#include <optional>
#include <string>

#define DEFAULT_NAME "player"
#define DEFAULT_URL "127.0.0.1"
#define DEFAULT_PORT 6767
#define DEFAULT_VOLUME "0.5"

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
  GameSettings settings{DEFAULT_NAME, DEFAULT_URL, DEFAULT_PORT, DEFAULT_VOLUME};

  /// @brief Scan the system for configuration directory
  /// @return Full path do this program configuration dir
  static std::filesystem::path getConfigPath();

public:
  /// @param path Manually set configuration path. Meant mostly for testing
  explicit ConfigManager(const std::optional<const std::filesystem::path> &path = std::nullopt);

  void save();
  void load();

  // === Setters, getters and shitters ===

  [[nodiscard]] const std::string &getPlayerName() const;
  void setPlayerName(const std::string &name);

  [[nodiscard]] const std::string &getServerUrl() const;
  void setServerUrl(const std::string &newUrl);

  [[nodiscard]] uint16_t getServerPort() const;
  void setServerPort(const uint16_t newPort);

  [[nodiscard]] const std::string &getVolumeLevel() const;
  void setVolumeLevel(const std::string &newVolumeLevel);
};
