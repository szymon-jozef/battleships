# === Entry point ===
add_executable(battleships)
target_sources(battleships
    PRIVATE
        src/main.cpp
)

target_link_libraries(battleships
    PRIVATE
        gui
        game_logic
        networking
        game_manager
        logging
        Boost::program_options
)

target_compile_definitions(battleships PUBLIC GAME_VERSION=\"${PROJECT_VERSION}\")
