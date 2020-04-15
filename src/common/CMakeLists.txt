find_package(Qt5 REQUIRED COMPONENTS Core Gui)

set ( CMAKE_AUTOMOC ON)

set ( common_sources
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

if(${MSVC})
	set ( common_sources
		${common_sources}
		compat/exports.def
	)
endif()

add_library( common SHARED 	${common_sources} )
target_link_libraries(common Qt5::Core Qt5::Gui)

if (${MSVC})
	set_target_properties(common PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()