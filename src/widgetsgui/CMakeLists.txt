find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set ( CMAKE_AUTOMOC ON )

set (
    SOURCES
    gvfcanvas/canvasscene.cpp
    gvfcanvas/layeritem.cpp 
    gvfcanvas/viewportwidget.cpp
    main/basedocumentview.cpp
    main/editorview.cpp
    main/viewportscrollwidget.cpp
    main/tooloptionsbars/brushtooloptionsbar.cpp
    main/tooloptionsbars/navigatetooloptionsbar.cpp
    main/tooloptionsbars/tooloptionbarbase.cpp
    main/zoomrotatewidget.cpp
    utilities/zoomslider.cpp
    utilities/optiongroup.cpp
)

add_library(widgetsgui SHARED ${SOURCES})
target_link_libraries(widgetsgui common Qt5::Core Qt5::Widgets )
