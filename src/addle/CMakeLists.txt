find_package(Qt5 REQUIRED COMPONENTS Core)

INCLUDE_DIRECTORIES(/usr/include/xcb)
LINK_DIRECTORIES(/usr/lib/xcb)

#find_package(xcb)

set (
    SOURCES
    addle.cpp
    serviceconfiguration.cpp
)

add_executable(addle ${SOURCES})
target_link_libraries(addle Qt5::Core xcb core common widgetsgui )
