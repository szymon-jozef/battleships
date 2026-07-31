# === Tests ===
find_package(Catch2 3 REQUIRED)

file(GLOB TEST_LOGIC_SOURCES CONFIGURE_DEPENDS "tests/game_logic/*.cpp")
add_executable(LogicTests ${TEST_LOGIC_SOURCES})

target_link_libraries(LogicTests
    PRIVATE
        Catch2::Catch2WithMain
        game_logic
)

file(GLOB TEST_INTEGRATION_SOURCES CONFIGURE_DEPENDS "tests/integration_tests/*.cpp")
add_executable(IntegrationTests ${TEST_INTEGRATION_SOURCES})

target_link_libraries(IntegrationTests
    PRIVATE
        Catch2::Catch2WithMain
        spdlog::spdlog
        networking
        game_manager
        game_logic
)
include(CTest)
include(Catch)
catch_discover_tests(LogicTests)
catch_discover_tests(IntegrationTests)

