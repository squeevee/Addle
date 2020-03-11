find_package(Qt5 REQUIRED COMPONENTS Core)

set ( CMAKE_AUTORCC ON )

set (
    SOURCES
    addle.cpp
    serviceconfiguration.cpp

    ../../resources/resources.qrc
)

add_executable(addle ${SOURCES})
target_link_libraries(addle Qt5::Core canvas core common widgetsgui )
