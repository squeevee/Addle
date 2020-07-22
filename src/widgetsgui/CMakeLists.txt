find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set ( CMAKE_AUTOMOC ON )

set (
    SOURCES
    canvas/docbackgrounditem.cpp
    canvas/canvasscene.cpp
    canvas/layeritem.cpp
    main/assetselector.cpp
    main/colorselector.cpp
    main/favoriteassetspicker.cpp
    main/maineditorview.cpp
    main/layers/layersmanager.cpp
    main/paletteview.cpp
    main/sizeselector.cpp
    main/viewportscrollwidget.cpp
    main/viewport.cpp
    main/zoomrotatewidget.cpp
    main/tooloptionsbars/brushtooloptionsbar.cpp
    main/tooloptionsbars/navigatetooloptionsbar.cpp
    main/tooloptionsbars/tooloptionbarbase.cpp
    utilities/colorwell.cpp
    utilities/guiutils.cpp
    utilities/graphicsmouseeventblocker.cpp
    utilities/optiongroup.cpp
    utilities/popupbutton.cpp
    utilities/zoomslider.cpp
)

add_definitions(-DADDLE_EXPORTING_WIDGETSGUI)
add_library(widgetsgui SHARED ${SOURCES})
target_link_libraries(widgetsgui common Qt5::Core Qt5::Widgets )
