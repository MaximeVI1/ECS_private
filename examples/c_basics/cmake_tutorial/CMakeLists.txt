cmake_minimum_required(VERSION 3.10)
project(cmake_tutorial LANGUAGES C)

# Target: library
add_library(mymath SHARED
    src/mymath/mymath.c
)
target_include_directories(mymath PUBLIC
    include)

# Target: executable
add_executable(app src/app.c)

# Target: executable
add_executable(test tests/test_wrap_angle_to_180.c)
target_link_libraries(test 
    mymath)
