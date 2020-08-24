find_package(Qt5 REQUIRED COMPONENTS Core)

include_directories(../core ../widgetsgui)

set (
    SOURCES
    addle.cpp
    serviceconfiguration.cpp
)

add_executable(addle ${SOURCES})
target_link_libraries(addle Qt5::Core addlecore addlecommon addlewidgetsgui )

if (WIN32)
    # Convenience for running Addle out of the build directory.
    add_custom_command (
        TARGET addle POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
			$<TARGET_FILE:addlecommon>
			$<TARGET_FILE:addlecore>
			$<TARGET_FILE:addlewidgetsgui>
			${EXECUTABLE_OUTPUT_PATH}
    )

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set_target_properties(addle PROPERTIES WIN32_EXECUTABLE true)
    endif()
endif ()

if (UNIX)
    install(TARGETS addle)
endif()