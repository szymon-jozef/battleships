#include "../src/config/config.hpp"

#include <catch2/catch_test_case_info.hpp>
#include <catch2/catch_test_macros.hpp>

#include <glaze/json.hpp>

#include <filesystem>

#include <cstdlib>
#include <raylib.h>

const std::filesystem::path genRandPath() {
  int randomNumber = rand();
  return std::filesystem::temp_directory_path() /
         "battleships_temp_test_dir_i_hope_that_another_directory_like_this_wont_exists_anywhere_in_the_"
         "slash_temp_directory" /
         std::to_string(randomNumber);
}

TEST_CASE("Config manager default values") {
  ConfigManager manager(genRandPath() / "defaults");
  manager.load();

  REQUIRE(manager.getPlayerName() == "player");
  REQUIRE(manager.getServerPort() == 6767);
  REQUIRE(manager.getServerUrl() == "127.0.0.1");
  REQUIRE(manager.getVolumeLevel() == "0.5");
}

TEST_CASE("Saving and reading config", "[ConfigManager]") {
  std::filesystem::path path = genRandPath() / "saving";
  ConfigManager manager(path);

  constexpr const char *testName = "morbius";
  constexpr uint16_t testPort = 6969;
  constexpr const char *testUrl = "https://example.com";
  constexpr const char *testVolumeLevel = "0.1";

  manager.setPlayerName(testName);
  manager.setServerPort(6969);
  manager.setServerUrl("https://example.com");
  manager.setVolumeLevel("0.1");
  manager.save();

  ConfigManager manager2(path);

  manager2.load();

  REQUIRE(manager.getPlayerName() == testName);
  REQUIRE(manager.getServerPort() == testPort);
  REQUIRE(manager.getServerUrl() == testUrl);
  REQUIRE(manager.getVolumeLevel() == testVolumeLevel);
}
