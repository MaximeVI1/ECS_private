cmake_minimum_required(VERSION 3.13.0)
project(math_rob_c VERSION 0.0.0 LANGUAGES C CXX)

###########
## Build ##
###########

add_library(${PROJECT_NAME} 
    SHARED
        src/numbers/numbers.c
        src/calculus/polynomial.c
)

target_include_directories(${PROJECT_NAME}  
    PUBLIC
        $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
)

target_link_libraries(${PROJECT_NAME}
    m
)

