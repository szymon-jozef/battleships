#pragma once
#include <filesystem>
#include <raylib.h>
#include <vector>

namespace battleship {
namespace gui {
int run();

class AssetsManager {
  std::filesystem::path pathPrefix;
  // we first change to string then to c_str, because windows is bad
  std::vector<std::filesystem::path> expectedPaths = {std::filesystem::path("/usr/share/battleship"),
                                                      std::filesystem::path(GetApplicationDirectory()) /
                                                          std::filesystem::path("./assets"),
                                                      std::filesystem::path("./assets")};
  void loadPaths();

public:
  Texture2D bg1;
  Texture2D bg2;
  Texture2D bg3;
  Texture2D playBackground;

  AssetsManager();
  ~AssetsManager();
};
} // namespace gui
} // namespace battleship
