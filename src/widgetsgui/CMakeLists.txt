find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set ( CMAKE_AUTOUIC ON )
set ( CMAKE_AUTOMOC ON )

set ( CMAKE_AUTOUIC_SEARCH_PATHS uic )

set (
    SOURCES
    utilities/zoomslider.cpp
    views/canvasview.cpp
    views/basedocumentview.cpp
    views/editorview.cpp
    views/viewport.cpp
)

add_library(widgetsgui SHARED ${SOURCES})
target_link_libraries(widgetsgui common Qt5::Core Qt5::Widgets )
