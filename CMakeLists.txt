# Addle configuration code
# @file
# @copyright Copyright 2020 Eleanor Hawk
# @copyright Modification and distribution permitted under the terms of the
# MIT License. See "LICENSE" for full details.

cmake_minimum_required(VERSION 3.4)
project(Addle)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DADDLE_DEBUG -DBOOST_DI_CFG_DIAGNOSTICS_LEVEL=2")
set(CMAKE_CXX_STANDARD 17)

if (CMAKE_COMPILER_IS_GNUCC)

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")

endif ()

if (WIN32)

    set(CMAKE_INSTALL_PREFIX, "C:\\Program Files\\Addle")

endif ()

include_directories(
    .
    src
    src/common
)

find_package( Qt5   5.15    REQUIRED COMPONENTS Core Gui)
find_package( Boost 1.75    REQUIRED )

add_subdirectory( src )

add_subdirectory( thirdparty/boost-ext-di )
# CMake Deprecation Warning at thirdparty/boost-ext-di/CMakeLists.txt:7 (cmake_minimum_required):
#   Compatibility with CMake < 2.8.12 will be removed from a future version of
#   CMake.
#
#   Update the VERSION argument <min> value or use a ...<max> suffix to tell
#   CMake that the project does not need compatibility with older versions.
#
# TODO: open an issue on [Boost::ext].DI GitHub

include( CTest )
if(BUILD_TESTING)
    add_subdirectory( tests )
endif()
