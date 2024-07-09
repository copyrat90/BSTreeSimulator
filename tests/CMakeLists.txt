cmake_minimum_required(VERSION 3.25)
project(BSTreeTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

add_executable(bstree_validate bstree_validate.cpp)
target_include_directories(bstree_validate PRIVATE ../src)
target_compile_options(bstree_validate PRIVATE ${bs_compile_options})

add_executable(rbtree_validate rbtree_validate.cpp)
target_include_directories(rbtree_validate PRIVATE ../src)
target_compile_options(rbtree_validate PRIVATE ${bs_compile_options})

add_executable(bheap_validate bheap_validate.cpp)
target_include_directories(bheap_validate PRIVATE ../src)
target_compile_options(bheap_validate PRIVATE ${bs_compile_options})
