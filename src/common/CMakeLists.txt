find_package(Qt5 REQUIRED COMPONENTS Core Gui Concurrent)
find_package(Boost REQUIRED )
find_library(LIBBACKTRACE libbacktrace.so libbacktrace.dll)

set (CMAKE_AUTOMOC ON)
set ( CMAKE_AUTORCC ON )

set (common_sources
    ${INTERFACES}
    
    common.cpp
    exceptions/addleexception.cpp
    utilities/asynctask.cpp
    #utilities/config/bindingfilter.cpp
    utilities/config3/configexceptions.cpp
    utilities/config3/interfacebindingconfig.cpp
    utilities/errors.cpp
    utilities/collatingstring.cpp
    utilities/coordinatedreadwritelock.cpp
    utilities/datatree/addledatatree.cpp
    utilities/datatree/observer.cpp
    utilities/iocheck.cpp
    utilities/indexvariant.cpp
    utilities/taskprogresshandle.cpp
    utilities/translatedstring.cpp
    utilities/taskprogresshandle.hpp
    utilities/editing/brushstroke.cpp
    utilities/image/rastertile.cpp
    utilities/image/rastertileedithandle.cpp
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

if (NOT LIBBACKTRACE-NOTFOUND)
    add_definitions(-DADDLE_USE_LIBBACKTRACE)
    target_link_libraries(addlecommon debug ${LIBBACKTRACE})
endif()

if (UNIX)
    install( TARGETS addlecommon )
endif()
