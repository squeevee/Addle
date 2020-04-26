find_package(Qt5 REQUIRED COMPONENTS Core)

set ( CMAKE_AUTORCC ON )

set (
    SOURCES
    addle.cpp
    serviceconfiguration.cpp

    ../../resources/resources.qrc
)

add_executable(addle ${SOURCES})
target_link_libraries(addle Qt5::Core core common widgetsgui )

if (WIN32)
    add_custom_command (
        TARGET addle POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
			$<TARGET_FILE:common>
			$<TARGET_FILE:core>
			$<TARGET_FILE:widgetsgui>
			${EXECUTABLE_OUTPUT_PATH}
    )

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set_target_properties(addle PROPERTIES WIN32_EXECUTABLE true)
    endif()

endif ()