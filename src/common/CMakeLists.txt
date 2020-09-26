find_package(Qt5 REQUIRED COMPONENTS Core Gui)
find_package(Boost REQUIRED)

set (CMAKE_AUTOMOC ON)
set ( CMAKE_AUTORCC ON )

set (common_sources
    ${INTERFACES}
    
    common.cpp
    servicelocator.cpp
    exceptions/addleexception.cpp
    utilities/asynctask.cpp
    utilities/configuration/serviceconfig.cpp
    utilities/errors.cpp
    utilities/iocheck.cpp
    utilities/indexvariant.cpp
    utilities/translatedstring.cpp
    utilities/editing/brushstroke.cpp
    utilities/format/genericformat.cpp
    utilities/image/rasterbithandles.cpp
    utilities/image/rasterpainthandle.cpp
    utilities/presenter/filerequest.cpp
    utilities/presenter/propertybinding.cpp
    utilities/presenter/propertyobserver.cpp
    utilities/render/rendersubstack.cpp
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
target_link_libraries(addlecommon Qt5::Core Qt5::Gui Boost::headers)

if (UNIX)
    install( TARGETS addlecommon )
endif()
