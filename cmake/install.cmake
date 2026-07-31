if (isRelease)
    install(TARGETS battleships DESTINATION .)
    install(DIRECTORY assets DESTINATION .)
else()
    install(TARGETS battleships DESTINATION bin)

    if (WIN32)
        install(DIRECTORY assets DESTINATION "C:/Program files/battleships")
    else()
        install(DIRECTORY assets DESTINATION share/battleships)
    endif()

endif()

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

if (WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)
