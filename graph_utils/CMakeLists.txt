

add_library(graph_utils STATIC
    src/ConnectedComponent.cpp
    src/DegreeAnalysis.cpp
)


target_include_directories(graph_utils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

# graph_utils 依赖 kica_core
target_link_libraries(graph_utils PUBLIC kica_core)