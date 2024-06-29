cmake_minimum_required(VERSION 3.25)
project(BSTreeTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

add_executable(bstree_validate bstree_validate.cpp)
target_include_directories(bstree_validate PRIVATE ../src)
target_compile_options(bstree_validate PRIVATE ${bs_compile_options})
