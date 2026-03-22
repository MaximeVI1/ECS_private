cmake_minimum_required(VERSION 3.5)
project(read_parameter_struct VERSION 0.0.1 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)

# Options
option(BUILD_EXAMPLES "Build examples" OFF)

###########
## Build ##
###########
add_subdirectory(external/cjson)

add_library(${PROJECT_NAME} 
    SHARED
        src/read_file.c   
)
add_dependencies(${PROJECT_NAME} cjson)
  
target_include_directories(${PROJECT_NAME}  
    PUBLIC
        include
        ${cJSON_SOURCE_DIR}
) 

target_link_directories(${PROJECT_NAME}
    PRIVATE
        ${cJSON_BINARY_DIR}
)
target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        cjson
)
    

install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
