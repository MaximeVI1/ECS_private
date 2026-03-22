cmake_minimum_required(VERSION 3.10.0)
project(five_c VERSION 0.1.0 LANGUAGES C CXX) 

message(STATUS "PROJECT_NAME: " ${PROJECT_NAME})

# CMake flags
option(BUILD_OFFLINE "Build offline" ON)

# CMake doesn't provide a GraphVIZ module so we add it locally
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
find_package(Graphviz REQUIRED)

###########
## Build ##
###########
# Current project
add_library(${PROJECT_NAME} SHARED
    src/activity.c
    src/thread.c
    src/schedule.c
    src/petrinet_scheduler.c
    src/flowchart_execution.c
)
add_dependencies(${PROJECT_NAME} coordination_libraries)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${GRAPHVIZ_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${UTHASH_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${coordination_libraries_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${data_structure_SOURCE_DIR}/include>
)

target_link_directories(${PROJECT_NAME}
    PRIVATE
        ${COORDINATION_LIBRARIES_BUILD_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${GRAPHVIZ_CDT_LIBRARY}
        ${GRAPHVIZ_CGRAPH_LIBRARY}
        coordination_libraries
)

####################################
#####         INSTALL         ######
####################################
install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})
install(TARGETS ${PROJECT_NAME}  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
