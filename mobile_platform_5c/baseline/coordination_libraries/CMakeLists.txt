cmake_minimum_required(VERSION 3.10.0)
project(coordination_libraries VERSION 0.0.1 LANGUAGES C CXX)

message(STATUS "PROJECT_NAME: " ${PROJECT_NAME})

# CMake flags
option(BUILD_EXAMPLES "Build all examples." OFF)

# CMake doesn't provide a GraphVIZ module so we add it locally
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
find_package(Graphviz REQUIRED)

###########
## Build ##
###########
add_library(${PROJECT_NAME} SHARED
    src/petrinet.c
    src/DAG.c
    src/lcsm.c
    src/FSM.c
    src/top-level_4_state_lcsm.c
)


target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${GRAPHVIZ_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${UTHASH_INCLUDE_DIR}>
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${GRAPHVIZ_CDT_LIBRARY}
        ${GRAPHVIZ_CGRAPH_LIBRARY}
)


#############
## Install ##
#############
install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})
install(TARGETS ${PROJECT_NAME}  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )


