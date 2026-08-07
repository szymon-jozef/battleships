#pragma once
#include <atomic>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <glaze/core/common.hpp>
#include <glaze/json.hpp>
#include <raylib.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>
#include <string_view>

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

struct GameSettings {
  std::string playerName{};
  std::string serverUrl{};
  uint16_t serverPort{};
  std::string volumeLevel{};
};

static_assert(glz::reflectable<GameSettings>);

class SettingsManager {
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
  GameSettings settings{"player", "127.0.0.1", 6767, "0.5"};

  SettingsManager() {
    loadPaths();
    configPath /= "battleships";

    std::filesystem::create_directories(configPath);

    configFilePath = configPath / std::filesystem::path("config.json");
    spdlog::info("[GUI] settings path is: {}", configFilePath.string());

    if (!std::filesystem::is_regular_file(configFilePath)) {
      std::ofstream file(configFilePath);
      file.close();
      save();
    } else {
      load();
    }

    // this can still be empty, because we don't change if volumeLevel entry even exists in the config file
    if (settings.volumeLevel.empty()) {
      settings.volumeLevel = "0.5";
      spdlog::warn("[GUI] volumeLevel field not found in the config file. Defaulting to 0.5");
    }

    SetMasterVolume(std::stof(settings.volumeLevel));
    spdlog::info("[GUI] game loaded with master volume at: {}", settings.volumeLevel);
  }

  void save() {
    std::string buffer{};
    glz::error_ctx ec;
    ec = glz::write_json(settings, buffer);

    if (ec) {
      spdlog::error("[GUI] While writing settings to json: {}", ec.custom_error_message);
      return;
    }

    std::ofstream file(configFilePath);

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

  void load() {
    std::ifstream file(configFilePath);

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

  const std::string &getPlayerName() const {
    return settings.playerName;
  }

  void setPlayerName(const std::string &name) {
    settings.playerName = name;
  }

  const std::string &getServerUrl() const {
    return settings.serverUrl;
  }

  void setServerUrl(const std::string &newUrl) {
    settings.serverUrl = newUrl;
  }

  uint16_t getServerPort() const {
    return settings.serverPort;
  }

  void setServerPort(const uint16_t newPort) {
    settings.serverPort = newPort;
  }

  const std::string &getVolumeLevel() const {
    return settings.volumeLevel;
  }

  void setVolumeLevel(const std::string &newVolumeLevel) {
    settings.volumeLevel = newVolumeLevel;
  }
};

class AssetsManager {
  std::filesystem::path pathPrefix;
  // we first change to string then to c_str, because windows is bad
  std::vector<std::filesystem::path> expectedPaths = {std::filesystem::path("/usr/share/battleship"),
                                                      std::filesystem::path(GetApplicationDirectory()) /
                                                          std::filesystem::path("./assets"),
                                                      std::filesystem::path("./assets")};
  void loadPaths() {
    bg1 = loadAsset<Texture2D>("gfx/bg1.png");
    bg2 = loadAsset<Texture2D>("gfx/bg2.png");
    bg3 = loadAsset<Texture2D>("gfx/bg3.png");
    playBackground = loadAsset<Texture2D>("gfx/play_background.png");

    click = loadAsset<Sound>("sfx/click.mp3");
    hit = loadAsset<Sound>("sfx/hit.ogg");
    sink = loadAsset<Sound>("sfx/sink.ogg");
    miss = loadAsset<Sound>("sfx/miss.ogg");
    start = loadAsset<Sound>("sfx/play.mp3");
  }

public:
  Texture2D bg1;
  Texture2D bg2;
  Texture2D bg3;
  Texture2D playBackground;

  Sound click;
  Sound hit;
  Sound miss;
  Sound sink;
  Sound start;

  AssetsManager() {
    InitAudioDevice();
    char *envPath = std::getenv("BATTLESHIPS_ASSETS_DIR");

    if (envPath) {
      pathPrefix = std::filesystem::path(envPath);
      spdlog::info("[GUI] Env variable BATTLESHIPS_ASSETS_DIR was set. Setting pathPrefix as: {}", envPath);
      loadPaths();
      return;
    }

    auto foundPathIterator =
        std::find_if(expectedPaths.begin(), expectedPaths.end(), [](const std::filesystem::path &p) {
          return std::filesystem::is_directory(p);
        });

    if (foundPathIterator == expectedPaths.end()) {
      spdlog::warn("[GUI] could not fine any viable asset directory");
      return;
    }

    pathPrefix = *foundPathIterator;
    spdlog::info("[GUI] found assets path at: {}", pathPrefix.string());
    loadPaths();
  }

  ~AssetsManager() {
    UnloadTexture(bg1);
    UnloadTexture(bg2);
    UnloadTexture(bg3);
    UnloadTexture(playBackground);

    UnloadSound(click);
    UnloadSound(hit);
    UnloadSound(miss);
    UnloadSound(sink);
    UnloadSound(start);
    CloseAudioDevice();
  }

  AssetsManager(const AssetsManager &as) = delete;
  AssetsManager &operator=(const AssetsManager &as) = delete;

private:
  template <typename T> T loadAsset(std::string_view subPath) {
    const auto assetPath = pathPrefix / std::filesystem::path{subPath};

    if constexpr (std::same_as<T, Texture2D>)
      return LoadTexture(assetPath.string().c_str());
    else if constexpr (std::same_as<T, Sound>)
      return LoadSound(assetPath.string().c_str());
    else
      static_assert(std::same_as<T, Texture2D> || std::same_as<T, Sound>);
  }
};

class GameContext {
public:
  GameContext()
      : assetsManager(AssetsManager()) {}

  AssetsManager assetsManager;
  SettingsManager gameSettings;

  std::string loserName;
  bool isWon = false;
  std::atomic<GuiState> guiState = GuiState::MAIN_MENU;
  enum class GameMode { HOSTING, JOINING };
  GameMode currentGameMode = GameMode::HOSTING;
};

} // namespace gui
} // namespace battleship
