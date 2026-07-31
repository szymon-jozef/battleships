#include "gui/gui.hpp"
#include "logging.hpp"

#include <boost/program_options.hpp>
#include <boost/program_options/detail/parsers.hpp>
#include <boost/program_options/options_description.hpp>
#include <iostream>
#include <spdlog/sinks/basic_file_sink-inl.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

int main(int ac, char **av) {
  boost::program_options::options_description desc("Allowe options");
  desc.add_options()("help", "produce help message")("version", "show version number")("log-stdout", "log to stdout");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);

  const std::string helpMsg = "Game of battleships\n\n"
                              "\tCLI options:\n"
                              "\t\t--help shows this message and returns\n"
                              "\t\t--version show version number and return\n"
                              "\t\t--log-stdout redirects all logs to stdout. This is usefull when your error is "
                              "related to raylib, since it will show both the game and raylib logs.\n";

  if (vm.count("help")) {
    std::cout << helpMsg;
    return 0;
  }

  if (vm.count("version")) {
    std::cout << "Battleships: v" << GAME_VERSION;
    return 0;
  }

  if (!vm.count("log-stdout")) {
    battleship::logger::setupSpdlogFileLogging();
  }

  spdlog::info("[MAIN] Running the game. Current version is: {}", GAME_VERSION);
  return battleship::gui::run();
}
