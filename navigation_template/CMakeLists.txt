cmake_minimum_required(VERSION 3.13.0)
project(navigation_template VERSION 0.0.0 LANGUAGES C CXX)

# In the future: change this flag name!! Its confusing
option(PLATFORM_CONTROL "Compile navigation activity with platform velocity control" ON)

#################################################################################
## Set this variable accordingly to obtain the path to "mobile_platform_5c" ##
#################################################################################
set(PATH_TO_MOBILE_PLATFORM_5C ../mobile_platform_5c)
message(STATUS "PATH_TO_MOBILE_PLATFORM_5C: " ${PATH_TO_MOBILE_PLATFORM_5C})

########################################
## Build mobile_platform_5c libraries ##
########################################
add_subdirectory(${PATH_TO_MOBILE_PLATFORM_5C} mobile_platform_5c)

###################
## Build project ##
###################
if (PLATFORM_CONTROL)
    set(NAVIGATION_ACTIVITY_SRC_FILE src/activity/navigation_with_platform_control.c)
    set(APP_SRC_FILE application/app_with_platform_control.c)
else()
    set(NAVIGATION_ACTIVITY_SRC_FILE src/activity/navigation_with_kelo_control.c)
    set(APP_SRC_FILE application/app_with_kelo_control.c)
endif()

message(STATUS "NAVIGATION ACTIVITY SOURCE FILE: " ${NAVIGATION_ACTIVITY_SRC_FILE})

add_library(${PROJECT_NAME}
    ${NAVIGATION_ACTIVITY_SRC_FILE}
)
add_dependencies(${PROJECT_NAME} five_c data_structure mobile_platform_logging)

target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${data_structure_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${five_c_SOURCE_DIR}/include>
)

target_link_directories(${PROJECT_NAME} 
    PRIVATE 
        ${data_structure_BINARY_DIR}
        ${five_c_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        mobile_platform_logging
        data_structure
        five_c
        lttng-ust
        dl

)

##########################
#    Application        ##
##########################
add_executable(application ${APP_SRC_FILE})
add_dependencies(application ${PROJECT_NAME} mobile_platform_5c)

target_link_libraries(application
    ${PROJECT_NAME}
    data_structure 
    five_c
#     lidar 
#     mobile_platform_control_5c
    mobile_platform_drive
    mobile_platform_proprioception_5c
    mobile_platform_logging
    m
    pthread
    read_parameter_struct
    lttng-ust
    dl
)

# Grant permission for executable to read/write via ethercat
IF ("$ENV{SUDO_COMMAND}" STREQUAL "")
    SET(SUDO_COMMAND sudo)    # default = sudo
ELSE ("$ENV{SUDO_COMMAND}" STREQUAL "")
    SET(SUDO_COMMAND $ENV{SUDO_COMMAND})
ENDIF ("$ENV{SUDO_COMMAND}" STREQUAL "")
add_custom_command(TARGET application
    POST_BUILD
    COMMAND ${SUDO_COMMAND} setcap cap_net_raw+ep $<TARGET_FILE:application>
)

if(BUILD_EXAMPLES)
    add_executable(example_logging example/example_logging.c)
    target_link_libraries(example_logging 
        PRIVATE
            ${PROJECT_NAME}
            pthread
            mobile_platform_logging
            coordination_libraries
            five_c
            data_structure
)
endif()