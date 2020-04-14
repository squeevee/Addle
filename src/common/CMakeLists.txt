find_package(Qt5 REQUIRED COMPONENTS Core Gui)

set ( CMAKE_AUTOMOC ON)

add_library( common SHARED 
    ${INTERFACES}
    staticdefinitions.cpp
    exceptions/baseaddleexception.cpp
    utilities/asynctask.cpp
    utilities/debugging.cpp
    utilities/indexvariant.cpp
    utilities/typeinforef.cpp
    utilities/unhandledexceptionrouter.cpp
    utilities/image/rasterbithandles.cpp
    utilities/image/rasterpainthandle.cpp
    utilities/presenter/propertybinding.cpp
    utilities/presenter/propertyobserver.cpp
    utilities/qtextensions/qhash.cpp
    utilities/qtextensions/translation.cpp
    utilities/render/rendersubstack.cpp
)
target_link_libraries(common Qt5::Core Qt5::Gui)