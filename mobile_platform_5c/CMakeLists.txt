cmake_minimum_required(VERSION 3.13.0)
project(mobile_platform_5c VERSION 0.0.0 LANGUAGES C CXX)
option(BUILD_EXAMPLES "Build all examples" OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

################################
##  Set path to 3rd party lib ##
################################
set(UTHASH_INCLUDE_DIR ${${PROJECT_NAME}_SOURCE_DIR}/baseline/uthash/include)

################################
## Build - baseline libraries ##
################################
# ROB-KUL libraries
add_subdirectory(baseline/coordination_libraries)
add_subdirectory(baseline/data_structure)
add_subdirectory(baseline/five_c)
add_subdirectory(baseline/read_parameter_struct)
# add_subdirectory(baseline/lidar)
add_subdirectory(baseline/math_rob_c)
# add_subdirectory(baseline/human_computer_interface)
add_subdirectory(baseline/mobile_platform_logging)
# add_subdirectory(baseline/mobile_platform_control)
add_subdirectory(baseline/mobile_platform_drive)
add_subdirectory(baseline/mobile_platform_proprioception)

# Dependencies
add_dependencies(five_c coordination_libraries)
# add_dependencies(lidar data_structure five_c read_parameter_struct)
# add_dependencies(human_computer_interface data_structure five_c)
add_dependencies(mobile_platform_logging data_structure five_c read_parameter_struct)
# add_dependencies(mobile_platform_control data_structure five_c math_rob_c read_parameter_struct) 
add_dependencies(mobile_platform_drive data_structure five_c read_parameter_struct mobile_platform_logging) 
add_dependencies(mobile_platform_proprioception data_structure five_c math_rob_c read_parameter_struct) 

###########################
##    Dummy target       ##
###########################
add_custom_target(mobile_platform_5c)
add_dependencies(mobile_platform_5c
    coordination_libraries
    data_structure 
    read_parameter_struct
    five_c 
#     lidar
    mobile_platform_drive
    mobile_platform_logging
#     human_computer_interface
)
