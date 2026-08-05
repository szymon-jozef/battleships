# === Logging ===
add_library(logging)
target_sources(logging
    PRIVATE
        "src/logging/logging.cpp"
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
        src/logging
    FILES
        src/logging/logging.hpp
)

target_link_libraries(logging
    PRIVATE
        spdlog::spdlog_header_only
        fmt::fmt
)

