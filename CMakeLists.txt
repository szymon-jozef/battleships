cmake_minimum_required(VERSION 3.28.3)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(THREADS_PREFER_PTHREAD_FLAG ON)

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/metadata.json METADATA_STRING)

string(JSON name GET ${METADATA_STRING} name)
string(JSON version GET ${METADATA_STRING} version)
string(JSON description GET ${METADATA_STRING} description)
string(JSON homepage_url GET ${METADATA_STRING} homepage_url)

if (WIN32)
    set(VCPKG_TARGET_TRIPLET x64-windows-static)
    set(BUILD_SHARED_LIBS OFF)
endif()

project(${name} 
    VERSION "${version}"
    DESCRIPTION "${description}"
    HOMEPAGE_URL "${homepage_url}"
    LANGUAGES CXX
)


if (WIN32)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

message(STATUS "=== Project metadata ===")
message(STATUS "Name: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Description: ${PROJECT_DESCRIPTION}")
message(STATUS "Source URL: ${PROJECT_HOMEPAGE_URL}")
message(STATUS "==============================")

option(isDev "build tests" false)
option(isRelease "install everything to ." false)




find_package(Boost REQUIRED COMPONENTS program_options)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(Threads REQUIRED)
find_package(raylib REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glaze REQUIRED)

include(cmake/battleships.cmake)
include(cmake/game_logic.cmake)
include(cmake/networking.cmake)
include(cmake/game_manager.cmake)
include(cmake/logging.cmake)
include(cmake/gui.cmake)
include(cmake/config.cmake)

if (isDev)
    include(cmake/tests.cmake)

    # show warnings if developing
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        target_compile_options(battleships PRIVATE -Wall -Wextra -Wunreachable-code)
        target_compile_options(gui PRIVATE -Wall -Wextra -Wunreachable-code)
        target_compile_options(networking PRIVATE -Wall -Wextra -Wunreachable-code)
        target_compile_options(game_logic PRIVATE -Wall -Wextra -Wunreachable-code)
        target_compile_options(game_manager PRIVATE -Wall -Wextra -Wunreachable-code)
    endif()
endif()


include(cmake/install.cmake)
