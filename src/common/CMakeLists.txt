find_package(Qt5 REQUIRED COMPONENTS Core Gui)

set ( CMAKE_AUTOMOC ON)

add_library( common SHARED 
    ${INTERFACES}
    servicelocator.cpp
    staticdefinitions.cpp
    exceptions/baseaddleexception.cpp
    utilities/debugging.cpp
    utilities/unhandledexceptionrouter.cpp
    utilities/image/bufferpainter.cpp
    utilities/image/expandingbuffer.cpp
    utilities/image/imagecompressor.cpp
    utilities/qt_extensions/qhash.cpp
    utilities/qt_extensions/translation.cpp
    utilities/qt_extensions/propertyobserver.cpp
)
target_link_libraries(common Qt5::Core Qt5::Gui)