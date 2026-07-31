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
