cmake_minimum_required(VERSION 4.1.2)
project(battleships)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

if (PROJECT_IS_TOP_LEVEL AND UNIX)
    # Create symlink to compile_commands.json for IDE to pick it up
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )
endif()

find_package(Boost REQUIRED)
find_package(Catch2 3 REQUIRED)

add_executable(battleships)

target_sources(battleships
    PRIVATE
        main.cpp
)

add_library(game_logic)

file(GLOB LOGIC_SOURCES CONFIGURE_DEPENDS "src/game_logic/*.cpp")

target_sources(game_logic
    PRIVATE
        ${LOGIC_SOURCES}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            src/game_logic
        FILES
            src/game_logic/logic_models.hpp
)

target_link_libraries(battleships
    PRIVATE
        game_logic
)

file(GLOB TEST_LOGIC_SOURCES CONFIGURE_DEPENDS "tests/game_logic/*.cpp")
add_executable(LogicTests ${TEST_LOGIC_SOURCES})

target_link_libraries(LogicTests
    PRIVATE
        Catch2::Catch2WithMain
        game_logic
)

include(CTest)
include(Catch)
catch_discover_tests(LogicTests)
