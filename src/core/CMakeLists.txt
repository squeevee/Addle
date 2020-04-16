find_package(Qt5 REQUIRED COMPONENTS Core Widgets Concurrent)

set ( CMAKE_AUTOMOC ON)

set (
    SOURCES
    editing/brushpainters/basicbrushpainter.cpp
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
    presenters/operationpresenters/brushoperationpresenter.cpp
    presenters/toolpresenters/toolpresenterbase.cpp
    presenters/toolpresenters/navigatetoolpresenter.cpp
    presenters/toolpresenters/brushtoolpresenter.cpp
    rendering/renderstack.cpp
    services/applicationservice.cpp
    services/formatservice.cpp
    format/qtformatdrivers/qtimageformatdriver.cpp
	definitions.cpp
)

add_definitions( -DADDLE_BUILDING_CORE )

add_library(core SHARED ${SOURCES})
target_link_libraries(core common Qt5::Core Qt5::Widgets)

if (${MSVC})
	set_target_properties(core PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()