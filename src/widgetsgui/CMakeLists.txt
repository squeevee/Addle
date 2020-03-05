find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set ( CMAKE_AUTOMOC ON )

set (
    SOURCES
    utilities/optiongroup.cpp
    views/canvasview.cpp
    views/basedocumentview.cpp
    views/editorview.cpp
    views/layerview.cpp
    views/viewport.cpp
    widgets/tooloptionsbars/brushtooloptionsbar.cpp
    widgets/tooloptionsbars/navigatetooloptionsbar.cpp
    widgets/tooloptionsbars/tooloptionbarbase.cpp
    widgets/viewportscrollwidget.cpp
    widgets/zoomrotatewidget.cpp
    widgets/zoomslider.cpp
)

add_library(widgetsgui SHARED ${SOURCES})
target_link_libraries(widgetsgui common Qt5::Core Qt5::Widgets )
