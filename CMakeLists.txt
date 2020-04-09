cmake_minimum_required (VERSION 2.6)
project (Addle)

set ( EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" )
set ( LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib" )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DADDLE_DEBUG")

include_directories(
    .
    src
    src/common
)

add_subdirectory( src )

include( CTest )

if (BUILD_TESTING)

    #add_subdirectory( tests )

endif()