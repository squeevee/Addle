find_package(Qt5 REQUIRED COMPONENTS Core Gui)
find_package(Boost REQUIRED)

set (CMAKE_AUTOMOC ON)
set ( CMAKE_AUTORCC ON )

set (common_sources
    ${INTERFACES}
    
    staticdefinitions.cpp
    exceptions/baseaddleexception.cpp
    utilities/asynctask.cpp
    utilities/debugging.cpp
    utilities/indexvariant.cpp
    utilities/typeinforef.cpp
    utilities/unhandledexceptionrouter.cpp
    utilities/editing/brushstroke.cpp
    utilities/image/rasterbithandles.cpp
    utilities/image/rasterpainthandle.cpp
    utilities/presenter/propertybinding.cpp
    utilities/presenter/propertyobserver.cpp
    utilities/render/rendersubstack.cpp

    ../../resources/resources.qrc
)

add_definitions(-DADDLE_EXPORTING_COMMON)

add_library( common SHARED 	${common_sources} )
target_link_libraries(common Qt5::Core Qt5::Gui)
