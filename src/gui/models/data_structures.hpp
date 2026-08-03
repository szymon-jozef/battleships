#pragma once
#include <atomic>
#include <charconv>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <raylib.h>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>
#include <type_traits>

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

  template <typename T> static void loadCasted(const std::string_view &dataToCast, T &target, const T defaultValue) {
    static_assert(std::is_arithmetic<T>::value, "Not an arithmetic type");

    auto [ptr, ec] = std::from_chars(dataToCast.data(), dataToCast.data() + dataToCast.size(), target);

    if (ec == std::errc::invalid_argument) {
      spdlog::error("[GUI] tried casting something that wasn't a number!");
      target = defaultValue;
    } else if (ec == std::errc::result_out_of_range) {
      spdlog::error("[GUI] numeric setting set as value out of range!");
      target = defaultValue;
    }
  }

public:
  std::string playerName;
  std::string serverUrl;
  uint16_t serverPort;
  std::string volumeLevel;

  GameSettings() {
    loadPaths();
    configPath /= "battleships";

    std::filesystem::create_directories(configPath);

    configFilePath = configPath / std::filesystem::path("config.cfg");
    spdlog::info("[GUI] settings path is: {}", configFilePath.string());

    if (!std::filesystem::is_regular_file(configFilePath)) {
      std::ofstream file(configFilePath);
      playerName = "player";
      serverUrl = "127.0.0.1";
      serverPort = 6767;
      volumeLevel = "0.5";
      file.close();
      save();
    } else {
      load();
    }

    SetMasterVolume(std::stof(volumeLevel));
    spdlog::info("[GUI] game loaded with master volume at: {}", volumeLevel);
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

    if (volumeLevel.empty()) {
      file << "volumeLevel=" << "0.5" << '\n';
    } else {
      file << "volumeLevel=" << volumeLevel << '\n';
    }

    file.close();
    spdlog::info("[GUI] settings have been saved");

    SetMasterVolume(std::stof(volumeLevel));
    spdlog::info("[GUI] master volume set at: {}", GetMasterVolume());
  }
  void load() {
    std::ifstream file(configFilePath);

    if (!file) {
      spdlog::error("[GUI] could not open config file before reading!");
      return;
    }

    std::string line;
    std::string_view line_view, type, value;

    while (std::getline(file, line)) {
      size_t delimeterPos;

      line_view = std::string_view(line);
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
        loadCasted(value, serverPort, static_cast<uint16_t>(6767));
      } else if (type == "volumeLevel") {
        if (value.empty()) {
          volumeLevel = "0.5";
        } else {
          volumeLevel = value;
        }
      } else {
        spdlog::warn("[GUI] unrecognize entry in the settings: {} - {}", type, value);
        continue;
      }
      spdlog::info("[GUI] setting have been loaded: {} - {}", type, value);
    }
  };
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
  GameSettings settings;

  std::string loserName;
  bool isWon = false;
  std::atomic<GuiState> guiState = GuiState::MAIN_MENU;
  enum class GameMode { HOSTING, JOINING };
  GameMode currentGameMode = GameMode::HOSTING;
};

} // namespace gui
} // namespace battleship
