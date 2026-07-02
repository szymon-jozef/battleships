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
find_package(spdlog  REQUIRED)
find_package(fmt  REQUIRED)
find_package(Threads REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(battleships)
target_sources(battleships
    PRIVATE
        main.cpp
)

# === Game logic ===
add_library(game_logic)
file(GLOB_RECURSE LOGIC_SOURCES CONFIGURE_DEPENDS "src/game_logic/*.cpp")
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

target_link_libraries(game_logic
    PUBLIC
    spdlog::spdlog
    fmt::fmt
)


# === Networking ===
add_library(networking)
file(GLOB NETWORKING_SOURCES CONFIGURE_DEPENDS "src/networking/*.cpp")
target_sources(networking
    PRIVATE
        ${NETWORKING_SOURCES}
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
        src/networking
    FILES
        src/networking/client.hpp
        src/networking/connection.hpp
        src/networking/data_types.hpp
        src/networking/messages.hpp
        src/networking/network_player.hpp
        src/networking/server.hpp

)

target_link_libraries(networking
    PUBLIC
    spdlog::spdlog
    fmt::fmt
    Boost::headers
    Threads::Threads
)

add_executable(server_main)
target_sources(server_main
    PRIVATE
        src/apps/server_main.cpp
)

add_executable(client_main)
target_sources(client_main
    PRIVATE
        src/apps/client_main.cpp
)

# === Linking ===
target_link_libraries(battleships
    PRIVATE
        game_logic
        networking
)

target_link_libraries(server_main
    PRIVATE
        networking
)

target_link_libraries(client_main
    PRIVATE
        networking
)


# === Tests ===
file(GLOB TEST_LOGIC_SOURCES CONFIGURE_DEPENDS "tests/game_logic/*.cpp")
add_executable(LogicTests ${TEST_LOGIC_SOURCES})

target_link_libraries(LogicTests
    PRIVATE
        Catch2::Catch2WithMain
        game_logic
)

target_compile_options(LogicTests PRIVATE -fsanitize=address -g)
target_link_options(LogicTests PRIVATE -fsanitize=address)

include(CTest)
include(Catch)
catch_discover_tests(LogicTests)
