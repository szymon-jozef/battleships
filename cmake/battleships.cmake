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

if (WIN32)
    target_compile_definitions(battleships PRIVATE WIN32_LEAN_AND_MEAN NOGDI NOUSER NOMINMAX _CRT_SECURE_NO_WARNINGS)
endif()