find_package(Qt5 REQUIRED COMPONENTS Core Quick Qml QuickWidgets)
# find_package(Qt5QuickCompiler)

set ( CMAKE_AUTOMOC ON )
set ( CMAKE_AUTORCC ON )

set (
    SOURCES
    viewport.cpp

    canvas.qrc
)

add_library(canvas SHARED ${SOURCES})
target_link_libraries(canvas Qt5::Core Qt5::Quick Qt5::Qml Qt5::QuickWidgets common )
