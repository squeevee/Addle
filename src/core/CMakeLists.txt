find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set ( CMAKE_AUTOMOC ON)

set (
    SOURCES
    models/document.cpp
    models/layer.cpp
    presenters/helpers/zoomrotatehelper.cpp
    presenters/basedocumentpresenter.cpp
    presenters/canvaspresenter.cpp
    presenters/editorpresenter.cpp
    presenters/errorpresenter.cpp
    presenters/viewportpresenter.cpp
    presenters/tools/toolpresenterbase.cpp
    presenters/tools/navigatetoolpresenter.cpp
    services/applicationservice.cpp
    services/formatservice.cpp
    services/taskservice.cpp
    format/qtformatdrivers/qtimageformatdriver.cpp
    tasks/loaddocumentfiletask.cpp
    tasks/taskcontroller.cpp
    tasks/taskbase.cpp
)

add_library(core SHARED ${SOURCES})
target_link_libraries(core common Qt5::Core Qt5::Widgets)