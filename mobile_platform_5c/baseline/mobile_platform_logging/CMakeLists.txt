cmake_minimum_required(VERSION 3.13.0)
project(mobile_platform_logging VERSION 0.0.1 LANGUAGES C CXX)

message(STATUS "PROJECT_NAME: " ${PROJECT_NAME})

###################
## Configuration ##
###################

OPTION(BUILD_OFFLINE "Build offline" ON)

if (${BUILD_OFFLINE})
    set_property(DIRECTORY ${${PROJECT_NAME}_SOURCE_DIR}
                 PROPERTY EP_UPDATE_DISCONNECTED 1)
endif()


#############################
## mobile_platform_logging ##
#############################
add_library(${PROJECT_NAME} 
    SHARED
        src/tracepoint/logging-tp.c
)
add_dependencies(${PROJECT_NAME}
    data_structure
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${data_structure_SOURCE_DIR}/include>
)

target_link_directories(${PROJECT_NAME}
    PRIVATE
        ${data_structure_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        data_structure
        lttng-ust
        dl
)
