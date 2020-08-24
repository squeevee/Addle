find_package(Qt5 REQUIRED COMPONENTS Core Gui)
find_package(Boost REQUIRED)

set (CMAKE_AUTOMOC ON)
set ( CMAKE_AUTORCC ON )

set (common_sources
    ${INTERFACES}
    
    common.cpp
    exceptions/addleexception.cpp
    utilities/asynctask.cpp
    utilities/debugging.cpp
    utilities/errors.cpp
    utilities/indexvariant.cpp
    utilities/translatedstring.cpp
    utilities/typeinforef.cpp
    utilities/unhandledexceptionrouter.cpp
    utilities/editing/brushstroke.cpp
    utilities/format/genericformat.cpp
    utilities/image/rasterbithandles.cpp
    utilities/image/rasterpainthandle.cpp
    utilities/presenter/propertybinding.cpp
    utilities/presenter/propertyobserver.cpp
    utilities/render/rendersubstack.cpp
    utilities/view/documentlayersitemmodel.cpp

    ../../resources/resources.qrc
)

add_definitions(-DADDLE_EXPORTING_COMMON)

add_library( addlecommon SHARED ${common_sources} )
target_link_libraries(addlecommon Qt5::Core Qt5::Gui Boost::headers)

if (UNIX)
    install( TARGETS addlecommon )
endif()
