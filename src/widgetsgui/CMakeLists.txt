# Addle configuration code
# @file
# @copyright Copyright 2020 Eleanor Hawk
# @copyright Modification and distribution permitted under the terms of the
# MIT License. See "LICENSE" for full details.

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

include_directories(.)

set ( CMAKE_AUTOMOC ON )

set (
    SOURCES
    widgetsgui_config.cpp
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
    main/messages/notificationdialog.cpp
    main/messages/fileissuedialog.cpp
    main/tooloptionsbars/brushtooloptionsbar.cpp
    main/tooloptionsbars/navigatetooloptionsbar.cpp
    main/tooloptionsbars/tooloptionbarbase.cpp
    utilities/colorwell.cpp
    utilities/guiutils.cpp
    utilities/graphicsmouseeventblocker.cpp
    utilities/iconlabel.cpp
    utilities/optiongroup.cpp
    utilities/popupbutton.cpp
    utilities/filedialoghelper.cpp
    utilities/zoomslider.cpp
)

add_definitions(-DADDLE_EXPORTING_WIDGETSGUI)
add_library(addlewidgetsgui SHARED ${SOURCES})
target_link_libraries(addlewidgetsgui addlecommon Qt5::Core Qt5::Widgets )

if (UNIX)
    install(TARGETS addlewidgetsgui)
endif()
