add_executable(graph_test graph/test1.cc ${CMAKE_SOURCE_DIR}/src/LoopTreeBuilder.cc ${CMAKE_SOURCE_DIR}/src/graph.cc)
target_include_directories(graph_test PRIVATE ${INCLUDE_DIRS})
target_compile_options(graph_test PRIVATE -g -DNDEBUG_DEV)