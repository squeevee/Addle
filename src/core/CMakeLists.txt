find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set ( CMAKE_AUTOMOC ON)

set (
    SOURCES
    editing/brushoperation.cpp
    editing/brushrenderers/basicbrushrenderer.cpp
    models/document.cpp
    models/layer.cpp
    models/brush.cpp
    presenters/basedocumentpresenter.cpp
    presenters/editorpresenter.cpp
    presenters/errorpresenter.cpp
    presenters/layerpresenter.cpp
    presenters/viewportpresenter.cpp
    presenters/assets/brushpresenter.cpp
    presenters/tools/toolpresenterbase.cpp
    presenters/tools/navigatetoolpresenter.cpp
    presenters/tools/brushtoolpresenter.cpp
    services/applicationservice.cpp
    services/formatservice.cpp
    services/taskservice.cpp
    services/brushrepository.cpp
    format/qtformatdrivers/qtimageformatdriver.cpp
    tasks/loaddocumentfiletask.cpp
    tasks/taskcontroller.cpp
)

add_library(core SHARED ${SOURCES})
target_link_libraries(core common Qt5::Core Qt5::Widgets)