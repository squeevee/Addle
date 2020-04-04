find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set ( CMAKE_AUTOMOC ON)

set (
    SOURCES
    editing/brushpainters/basicbrushpainter.cpp
    editing/operations/rasteroperation.cpp
    editing/surfaces/rastersurface.cpp
    models/document.cpp
    models/layer.cpp
    presenters/canvaspresenter.cpp
    presenters/documentpresenter.cpp
    presenters/errorpresenter.cpp
    presenters/layerpresenter.cpp
    presenters/maineditorpresenter.cpp
    presenters/viewportpresenter.cpp
    presenters/assets/brushpresenter.cpp
    presenters/operationpresenters/brushoperationpresenter.cpp
    presenters/toolpresenters/toolpresenterbase.cpp
    presenters/toolpresenters/navigatetoolpresenter.cpp
    presenters/toolpresenters/brushtoolpresenter.cpp
    rendering/renderstack.cpp
    services/applicationservice.cpp
    services/formatservice.cpp
    services/taskservice.cpp
    format/qtformatdrivers/qtimageformatdriver.cpp
    tasks/loaddocumentfiletask.cpp
    tasks/taskcontroller.cpp
)

add_library(core SHARED ${SOURCES})
target_link_libraries(core common Qt5::Core Qt5::Widgets)