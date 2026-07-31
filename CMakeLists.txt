cmake_minimum_required(VERSION 3.28.3)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(THREADS_PREFER_PTHREAD_FLAG ON)

project(battleships 
    VERSION 0.0.6
    DESCRIPTION "Online game of battleships"
    HOMEPAGE_URL "https://github.com/szymon-jozef/battleships"
    LANGUAGES CXX
)

option(isDev "build tests" false)
option(isRelease "install everything to ." false)

find_package(Boost REQUIRED COMPONENTS program_options)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(Threads REQUIRED)
find_package(raylib REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

include(cmake/battleships.cmake)
include(cmake/game_logic.cmake)
include(cmake/networking.cmake)
include(cmake/game_manager.cmake)
include(cmake/logging.cmake)
include(cmake/gui.cmake)

if (isDev)
    include(cmake/tests.cmake)
endif()

if(WIN32)
    include(cmake/windows.cmake)
endif()

include(cmake/install.cmake)

# show warnings if developing
if (isDev AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
target_compile_options(battleships PRIVATE -Wall -Wextra -Wunreachable-code)
target_compile_options(gui PRIVATE -Wall -Wextra -Wunreachable-code)
target_compile_options(networking PRIVATE -Wall -Wextra -Wunreachable-code)
target_compile_options(game_logic PRIVATE -Wall -Wextra -Wunreachable-code)
target_compile_options(game_manager PRIVATE -Wall -Wextra -Wunreachable-code)
endif()

