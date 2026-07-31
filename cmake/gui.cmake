# === GUI ===
add_library(gui)
file(GLOB_RECURSE GUI_HEADERS CONFIGURE_DEPENDS "src/gui/*.hpp")
file(GLOB_RECURSE GUI CONFIGURE_DEPENDS "src/gui/*.cpp")

target_sources(
    gui
    PRIVATE
        ${GUI}
    PUBLIC
        FILE_SET HEADERS
    BASE_DIRS
        src/gui
    FILES
        ${GUI_HEADERS}
)

target_link_libraries(gui
    PRIVATE
        raylib
        glfw
        networking
        game_manager
        game_logic
        spdlog::spdlog
)
