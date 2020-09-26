# Addle configuration code
# @file
# @copyright Copyright 2020 Eleanor Hawk
# @copyright Modification and distribution permitted under the terms of the
# MIT License. See "LICENSE" for full details.

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Concurrent)

include_directories(.)

set ( CMAKE_AUTOMOC ON)

set (
    SOURCES
    core.cpp
    core_config.cpp
    editing/brushengines/pathbrushengine.cpp
    editing/brushengines/rasterbrushengine.cpp
    editing/rasterdiff.cpp
    editing/rastersurface.cpp
    models/brush.cpp
    models/document.cpp
    models/layer.cpp
    models/palette.cpp
    presenters/canvaspresenter.cpp
    presenters/documentpresenter.cpp
    presenters/layerpresenter.cpp
    presenters/maineditorpresenter.cpp
    presenters/viewportpresenter.cpp
    presenters/assets/brushpresenter.cpp
    presenters/colorselectionpresenter.cpp
    presenters/palettepresenter.cpp
    presenters/messages/notificationpresenter.cpp
    presenters/helpers/brushiconhelper.cpp
    presenters/helpers/loadhelper.cpp
    presenters/helpers/savetask.cpp
    presenters/operations/brushoperationpresenter.cpp
    presenters/tools/assetselectionpresenter.cpp
    presenters/tools/navigatetoolpresenter.cpp
    presenters/tools/sizeselectionpresenter.cpp
    presenters/tools/brushtoolpresenter.cpp
    rendering/renderstack.cpp
    services/appearanceservice.cpp
    services/applicationservice.cpp
    services/errorservice.cpp
    services/formatservice.cpp
    format/qtimageformatdriver.cpp
)

add_definitions(-DADDLE_EXPORTING_CORE)

add_library(addlecore SHARED ${SOURCES})
target_link_libraries(addlecore addlecommon Qt5::Core Qt5::Widgets Qt5::Concurrent)

if (UNIX)
    install (TARGETS addlecore)
endif()
