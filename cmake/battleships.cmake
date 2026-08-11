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
    set_property(TARGET battleships PROPERTY WIN32_EXECUTABLE ON)
    set_target_properties(battleships PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()