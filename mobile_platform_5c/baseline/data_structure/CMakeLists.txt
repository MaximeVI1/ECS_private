cmake_minimum_required(VERSION 3.10.0)
project(data_structure VERSION 1.0.0 LANGUAGES C CXX)
message(STATUS "PROJECT_NAME: " ${PROJECT_NAME})

OPTION(BUILD_EXAMPLES "Build all examples" OFF)

###########
## Build ##
###########
add_library(${PROJECT_NAME} SHARED
    src/registration/hash_table_utils.c
    src/registration/hash_table_variable.c
)    

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${UTHASH_INCLUDE_DIR}>
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        pthread
)
    
#############
##  TEST   ##
#############
if(BUILD_EXAMPLES)
    add_executable(registration_01 test/registration_01.c)
    target_link_libraries(registration_01
        ${PROJECT_NAME}
        pthread
    )
endif()

#############
## Install ##
#############
install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})
install(TARGETS ${PROJECT_NAME}  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
