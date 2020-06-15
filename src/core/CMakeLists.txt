find_package(Qt5 REQUIRED COMPONENTS Core Widgets Concurrent)

set ( CMAKE_AUTOMOC ON)

set (
    SOURCES
    definitions.cpp
    editing/brushengines/pathbrushengine.cpp
    editing/brushengines/rasterbrushengine.cpp
    editing/rasterdiff.cpp
    editing/rastersurface.cpp
    models/document.cpp
    models/layer.cpp
    presenters/canvaspresenter.cpp
    presenters/documentpresenter.cpp
    presenters/errorpresenter.cpp
    presenters/layerpresenter.cpp
    presenters/maineditorpresenter.cpp
    presenters/viewportpresenter.cpp
    presenters/assets/brushpresenter.cpp
    presenters/helpers/brushiconhelper.cpp
    presenters/operationpresenters/brushoperationpresenter.cpp
    presenters/toolpresenters/assetselectionpresenter.cpp
    presenters/toolpresenters/toolpresenterbase.cpp
    presenters/toolpresenters/navigatetoolpresenter.cpp
    presenters/toolpresenters/sizeselectionpresenter.cpp
    presenters/toolpresenters/brushtoolpresenter.cpp
    rendering/renderstack.cpp
    services/appearanceservice.cpp
    services/applicationservice.cpp
    services/i18nservice.cpp
    services/formatservice.cpp
    format/qtimageformatdriver.cpp
)

add_definitions(-DADDLE_EXPORTING_CORE)

add_library(core SHARED ${SOURCES})
target_link_libraries(core common Qt5::Core Qt5::Widgets Qt5::Concurrent)