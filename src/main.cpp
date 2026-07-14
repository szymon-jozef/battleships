#include "gui/main.cpp"
#include <boost/program_options.hpp>
#include <boost/program_options/detail/parsers.hpp>
#include <boost/program_options/options_description.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

using namespace battleship::gui;

int main(int ac, char **av) {
  boost::program_options::options_description desc("Allowe options");
  desc.add_options()("help", "produce help message")("log-stdout", "log to stdout");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);

  if (vm.count("help")) {
    std::cout << "description";
    return 1;
  }

  if (vm.count("log-stdout")) {
    std::cout << "logging to stdout :)";
    return 1;
  }

  spdlog::info("[MAIN] Running the game. Current version is: {}", GAME_VERSION);
  return run();
}
