add_executable(graph_test graph/test1.cc ${CMAKE_SOURCE_DIR}/src/LoopTreeBuilder.cc ${CMAKE_SOURCE_DIR}/src/graph.cc)
target_include_directories(graph_test PRIVATE ${INCLUDE_DIRS})
target_compile_options(graph_test PRIVATE -g -DNDEBUG_DEV)

add_executable(dfg_test peepholes_const_foldprop.cc ${CMAKE_SOURCE_DIR}/src/graph.cc ${CMAKE_SOURCE_DIR}/src/bbGraph.cc ${CMAKE_SOURCE_DIR}/src/basicblock.cpp ${CMAKE_SOURCE_DIR}/src/instruction.cc)
target_include_directories(dfg_test PRIVATE ${INCLUDE_DIRS})
target_compile_options(dfg_test PRIVATE -g -DNDEBUG_DEV)