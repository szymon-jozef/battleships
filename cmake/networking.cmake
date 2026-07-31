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
