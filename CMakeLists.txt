cmake_minimum_required(VERSION 3.10)

project (vmem VERSION 1.0.0 LANGUAGES C)

# public
add_library(vmem "src/vmem.c")
target_include_directories(vmem PUBLIC "inc")
# Test
add_executable(test "src/main.c")
target_link_libraries(test vmem)