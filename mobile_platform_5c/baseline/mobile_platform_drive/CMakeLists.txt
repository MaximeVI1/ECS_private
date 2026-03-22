cmake_minimum_required(VERSION 3.10.0)
project(mobile_platform_drive VERSION 0.1.0 LANGUAGES C CXX)

###################
## Configuration ##
###################
OPTION(BUILD_EXAMPLES "Build all examples" OFF)
OPTION(USE_SETCAP "Set permissions to access ethernet interface without sudo" ON)

# Optionally apply cap_net_raw+ep permissions to binary. Requires root privileges.
# Use sudo as default. Still, allow to be configured as environment variable.
IF ("$ENV{SUDO_COMMAND}" STREQUAL "")
    SET(SUDO_COMMAND sudo)    # default = sudo
ELSE ("$ENV{SUDO_COMMAND}" STREQUAL "")
    SET(SUDO_COMMAND $ENV{SUDO_COMMAND})
ENDIF ("$ENV{SUDO_COMMAND}" STREQUAL "")

if (${BUILD_OFFLINE})
    set_property(DIRECTORY ${${PROJECT_NAME}_SOURCE_DIR}
                 PROPERTY EP_UPDATE_DISCONNECTED 1)
endif()

###########
## Build ##
###########
# Third-package libraries which ROB-KUL does not have control
add_subdirectory(external/SOEM)

add_library(${PROJECT_NAME} STATIC
    src/activity/kelo_drive_activity.c
    src/kelo/kelo_soem.c
)
add_dependencies(${PROJECT_NAME} soem five_c data_structure read_parameter_struct)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${five_c_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${data_structure_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${read_parameter_struct_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${SOEM_SOURCE_DIR}/soem>
        $<BUILD_INTERFACE:${SOEM_SOURCE_DIR}/osal>
        $<BUILD_INTERFACE:${SOEM_SOURCE_DIR}/osal/linux>
        $<BUILD_INTERFACE:${SOEM_SOURCE_DIR}/oshw/linux>
)

target_link_directories(${PROJECT_NAME}
    PUBLIC  # PUBLIC becase soem path needs to be available
        ${five_c_BUILD_DIR}
        ${data_structure_BUILD_DIR}
        ${read_parameter_struct_BUILD_DIR}
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        soem
        data_structure
        five_c
        read_parameter_struct
        mobile_platform_logging
        lttng-ust
        dl
)

if(BUILD_EXAMPLES)
    add_executable(kelo_demo_command_line examples/kelo_demo_command_line.c)
    target_link_libraries(kelo_demo_command_line PRIVATE
        ${PROJECT_NAME}
        five_c
        pthread
        read_parameter_struct
    )

    # @TODO: This application has broken after the last update of the kelo_activity. 
    # Please updated it. Do not forget to uncomment the lines 96-99
    add_executable(kelo_demo_command_line_with_log examples/kelo_demo_command_line_with_log.c)
    target_link_libraries(kelo_demo_command_line_with_log PRIVATE
        ${PROJECT_NAME}
        five_c
        pthread
        read_parameter_struct
    )

    add_executable(kelo_demo_reportIV examples/kelo_demo_reportIV.c)
    target_link_libraries(kelo_demo_reportIV PRIVATE
        ${PROJECT_NAME}
        five_c
        pthread
        read_parameter_struct
    )

    add_executable(kelo_demo_changing_mode examples/kelo_demo_changing_mode.c)
    target_link_libraries(kelo_demo_changing_mode PRIVATE
        ${PROJECT_NAME}
        five_c
        pthread
        read_parameter_struct
    )
    IF (USE_SETCAP)
    add_custom_command(TARGET kelo_demo_command_line
            POST_BUILD
            COMMAND ${SUDO_COMMAND} setcap cap_net_raw+ep $<TARGET_FILE:kelo_demo_command_line>
    )
   
  
    add_custom_command(TARGET kelo_demo_command_line_with_log
            POST_BUILD
            COMMAND ${SUDO_COMMAND} setcap cap_net_raw+ep $<TARGET_FILE:kelo_demo_command_line_with_log>
    )

    add_custom_command(TARGET kelo_demo_reportIV
            POST_BUILD
            COMMAND ${SUDO_COMMAND} setcap cap_net_raw+ep $<TARGET_FILE:kelo_demo_reportIV>
    )


    add_custom_command(TARGET kelo_demo_changing_mode
            POST_BUILD
            COMMAND ${SUDO_COMMAND} setcap cap_net_raw+ep $<TARGET_FILE:kelo_demo_changing_mode>
    )
    endif (USE_SETCAP)
endif()

#############
## Install ##
#############
install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)



