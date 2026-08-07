#pragma once
#include "../config/config.hpp"

#include <atomic>
#include <filesystem>
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
  AssetsManager assetsManager;
  SettingsManager gameSettings;

  GameContext()
      : assetsManager(AssetsManager()) {
    const std::string volLvl = gameSettings.getVolumeLevel();
    if (!volLvl.empty()) {
      try {
        float vol = std::stof(volLvl);

        if (vol >= 0.0f && vol <= 1.0f) {
          SetMasterVolume(vol);
          spdlog::info("[GUI] master volume loaded and set at: {}", vol);
        }
      } catch (const std::invalid_argument &e) {
        spdlog::warn("[GUI] could not set master volume: ", e.what());
      }
    }
  }

  std::string loserName;
  bool isWon = false;
  std::atomic<GuiState> guiState = GuiState::MAIN_MENU;
  enum class GameMode { HOSTING, JOINING };
  GameMode currentGameMode = GameMode::HOSTING;
};

} // namespace gui
} // namespace battleship
