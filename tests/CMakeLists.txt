add_executable(graph_test graph/test1.cc)
target_include_directories(graph_test PRIVATE ${INCLUDE_DIRS})
target_compile_options(graph_test PRIVATE -g)