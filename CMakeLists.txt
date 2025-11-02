cmake_minimum_required(VERSION 3.10)

project (vm VERSION 1.0.0 LANGUAGES C)

add_library(vm "src/vm.c")
target_include_directories(vm PUBLIC "inc")
add_executable(exec "src/main.c")
target_link_libraries(exec vm)