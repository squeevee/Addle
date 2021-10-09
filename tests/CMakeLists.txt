# Addle configuration code
# @file
# @copyright Copyright 2020 Eleanor Hawk
# @copyright Modification and distribution permitted under the terms of the
# MIT License. See "LICENSE" for full details.

find_package( Qt5 REQUIRED COMPONENTS Core Gui Test Concurrent )

set(CMAKE_AUTOMOC ON)

set(
    COMMON_DEPS
    addlecommon
    Qt5::Core
    Qt5::Gui
    Qt5::Test
    Qt5::Concurrent
)

set(
    CORE_DEPS
    ${COMMON_DEPS}
    addlecore
)

include_directories(.)

macro(addle_common_test name)

    add_executable( ${name} ${CMAKE_CURRENT_LIST_DIR}/common/${name}.cpp )
    target_link_libraries( ${name} ${COMMON_DEPS})
    set_target_properties(
        ${name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin/common"
        RUNTIME_OUTPUT_NAME ${name}
    )

    set(COMMON_TESTS_TARGET ${COMMON_TESTS_TARGET} ${name})
    set(ALL_TESTS_TARGET ${ALL_TESTS_TARGET} ${name})

    add_test( NAME ${name} COMMAND ${name} )
endmacro()

macro(addle_core_test name)

    add_executable( ${name} ${CMAKE_CURRENT_LIST_DIR}/core/${name}.cpp )
    target_link_libraries( ${name} ${CORE_DEPS})
    set_target_properties(
        ${name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin/core"
        RUNTIME_OUTPUT_NAME ${name}
    )

    set(CORE_TESTS_TARGET ${CORE_TESTS_TARGET} ${name})
    set(ALL_TESTS_TARGET ${ALL_TESTS_TARGET} ${name})

    add_test( NAME ${name} COMMAND ${name} )
endmacro()

addle_common_test( datatree_utest       )
addle_common_test( presetmap_utest      )
addle_common_test( renderroutine_utest  )
addle_common_test( config3_utest        )

addle_core_test( rastersurface_utest )

add_custom_target( all_tests        DEPENDS ${ALL_TESTS_TARGET}     )
add_custom_target( common_tests     DEPENDS ${COMMON_TESTS_TARGET}  )
add_custom_target( core_tests       DEPENDS ${CORE_TESTS_TARGET}    )
