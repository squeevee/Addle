find_package(Qt5 REQUIRED COMPONENTS Core Gui Concurrent)
find_package(Boost REQUIRED )

set (CMAKE_AUTOMOC ON)
set ( CMAKE_AUTORCC ON )

set (common_sources
    ${INTERFACES}
    
    common.cpp
    exceptions/addleexception.cpp
    utilities/asynctask.cpp
    #utilities/config/bindingfilter.cpp
    utilities/errors.cpp
    utilities/collatingstring.cpp
    utilities/datatree/addledatatree.cpp
    utilities/datatree/observer.cpp
    utilities/iocheck.cpp
    utilities/indexvariant.cpp
    utilities/taskprogresshandle.cpp
    utilities/translatedstring.cpp
    utilities/taskprogresshandle.hpp
    utilities/editing/brushstroke.cpp
    utilities/format/genericformat.cpp
    utilities/image/rasterbithandles.cpp
    utilities/image/rasterpainthandle.cpp
    utilities/image/rastertile.cpp
    utilities/image/tilesurface.cpp
    utilities/render/renderroutine.cpp
    utilities/presenter/filerequest.cpp
    utilities/presenter/propertybinding.cpp
    utilities/presenter/propertyobserver.cpp
    utilities/view/documentlayersitemmodel.cpp

    ../../resources/resources.qrc
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (common_sources
        ${common_sources}
        utilities/debugging/debugbehavior.cpp
    )
endif()

add_definitions(-DADDLE_EXPORTING_COMMON)

add_library( addlecommon SHARED ${common_sources} )
target_link_libraries(addlecommon Qt5::Core Qt5::Gui Boost::headers Boost.DI)

if (UNIX)
    install( TARGETS addlecommon )
endif()
