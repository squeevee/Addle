# Addle configuration code
# @file
# @copyright Copyright 2020 Eleanor Hawk
# @copyright Modification and distribution permitted under the terms of the
# MIT License. See "LICENSE" for full details.

find_package( Qt5 REQUIRED COMPONENTS Core Gui Test )

set(CMAKE_AUTOMOC ON)

set(
    COMMON_DEPS
    common
    Qt5::Core
    Qt5::Gui
    Qt5::Test
)

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

addle_common_test( heirarchylist_utest )
addle_common_test( presethelper_utest )

add_custom_target( all_tests DEPENDS ${ALL_TESTS_TARGET} )
add_custom_target( common_tests DEPENDS ${COMMON_TESTS_TARGET} )