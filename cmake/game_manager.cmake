# === Game Manager ===
add_library(game_manager)
target_sources(
    game_manager
    PRIVATE
        src/game_manager/game_manager.cpp
    PUBLIC
        FILE_SET HEADERS
    BASE_DIRS
        src/game_manager
    FILES
        src/game_manager/game_manager.hpp
)

target_link_libraries(game_manager
    PUBLIC
    game_logic
    networking
)
