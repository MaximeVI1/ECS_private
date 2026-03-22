cmake_minimum_required(VERSION 3.13.0)
project(mobile_platform_proprioception VERSION 0.0.2 LANGUAGES C CXX)

#####################################
## mobile_platform_proprioception ##
####################################
add_library(${PROJECT_NAME} SHARED
    src/delta_tricycle.c
)
add_dependencies(${PROJECT_NAME} data_structure math_rob_c)
target_include_directories(${PROJECT_NAME} 
    PUBLIC
        $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${data_structure_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${math_rob_c_SOURCE_DIR}/include>
)

target_link_directories(${PROJECT_NAME} 
    PRIVATE
        ${data_structure_BINARY_DIR}
        ${math_rob_c_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        data_structure
        math_rob_c
        m
)

#######################################
## mobile_platform_proprioception_5c ##
#######################################
add_library(${PROJECT_NAME}_5c SHARED
    src/activity/activity.c
)
add_dependencies(${PROJECT_NAME}_5c ${PROJECT_NAME} 
    five_c data_structure read_parameter_struct)
target_include_directories(${PROJECT_NAME}_5c
    PUBLIC
        $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${five_c_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${data_structure_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${read_parameter_struct_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${math_rob_c_SOURCE_DIR}/include>>
)

target_link_directories(${PROJECT_NAME}_5c 
    PRIVATE
        ${${PROJECT_NAME}_BINARY_DIR}
        ${five_c_BINARY_DIR}
        ${data_structure_BINARY_DIR}
        ${read_parameter_struct_BINARY_DIR}
)
target_link_libraries(${PROJECT_NAME}_5c
    five_c
    data_structure
    mobile_platform_proprioception
    pthread
    read_parameter_struct
)
