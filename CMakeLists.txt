# Addle configuration code
# @file
# @copyright Copyright 2020 Eleanor Hawk
# @copyright Modification and distribution permitted under the terms of the
# MIT License. See "LICENSE" for full details.

cmake_minimum_required (VERSION 3.4)
project (Addle)

set ( EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" )
set ( LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib" )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DADDLE_DEBUG")

if (CMAKE_COMPILER_IS_GNUCC)

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")

endif ()

include_directories(
    .
    src
    src/common
)

find_package(Qt5Core REQUIRED)

add_subdirectory( src )

include( CTest )
if(BUILD_TESTING)
    add_subdirectory( tests )
endif()