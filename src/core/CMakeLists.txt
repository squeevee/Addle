find_package(Qt5 REQUIRED COMPONENTS Core Widgets Concurrent)

include_directories(.)

set ( CMAKE_AUTOMOC ON)

set (
    SOURCES
    core.cpp
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
    presenters/errorpresenter.cpp
    presenters/layerpresenter.cpp
    presenters/maineditorpresenter.cpp
    presenters/viewportpresenter.cpp
    presenters/assets/brushpresenter.cpp
    presenters/colorselectionpresenter.cpp
    presenters/palettepresenter.cpp
    presenters/operations/brushoperationpresenter.cpp
    presenters/helpers/brushiconhelper.cpp
    presenters/tools/assetselectionpresenter.cpp
    presenters/tools/navigatetoolpresenter.cpp
    presenters/tools/sizeselectionpresenter.cpp
    presenters/tools/brushtoolpresenter.cpp
    rendering/renderstack.cpp
    services/appearanceservice.cpp
    services/applicationservice.cpp
    services/formatservice.cpp
    format/qtimageformatdriver.cpp
)

add_definitions(-DADDLE_EXPORTING_CORE)

add_library(core SHARED ${SOURCES})
target_link_libraries(core common Qt5::Core Qt5::Widgets Qt5::Concurrent)