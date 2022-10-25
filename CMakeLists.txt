cmake_minimum_required(VERSION 3.10)

project (IR VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT DEFINED CATCH_TESTLIB_DIR)
    message(FATAL_ERROR "CATCH_TESTLIB_DIR is not defined")

endif()

set(INCLUDE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/src)

list(APPEND INCLUDE_DIRS ${INCLUDE_PREFIX};${CATCH_TESTLIB_DIR})

set(src src/basicblock.cpp src/bbGraph.cc src/graph.cpp src/instruction.cc src/main.cc)
add_executable(main ${src})
target_include_directories(main PRIVATE ${INCLUDE_DIRS})

# tests
add_subdirectory(tests)

