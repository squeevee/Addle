find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set ( CMAKE_AUTOMOC ON )

set (
    SOURCES
    canvas/docbackgrounditem.cpp
    canvas/canvasscene.cpp
    canvas/layeritem.cpp
    main/maineditorview.cpp
    main/viewportscrollwidget.cpp
    main/viewport.cpp
    main/zoomrotatewidget.cpp
    main/tooloptionsbars/brushtooloptionsbar.cpp
    main/tooloptionsbars/navigatetooloptionsbar.cpp
    main/tooloptionsbars/tooloptionbarbase.cpp
    utilities/guiutils.cpp
    utilities/graphicsmouseeventblocker.cpp
    utilities/optiongroup.cpp
    utilities/zoomslider.cpp
)

add_definitions( -DADDLE_BUILDING_WIDGETSGUI )

add_library(widgetsgui SHARED ${SOURCES})
target_link_libraries(widgetsgui common Qt5::Core Qt5::Widgets )

if (${MSVC})
	set_target_properties(widgetsgui PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()