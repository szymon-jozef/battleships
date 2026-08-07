add_library(config)
target_sources(config
    PRIVATE
        src/config/config.cpp
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
        src/config
    FILES
        src/config/config.hpp
)

target_link_libraries(config
    PUBLIC
        spdlog::spdlog_header_only
        fmt::fmt-header-only
        glaze::glaze
)
