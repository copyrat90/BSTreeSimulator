cmake_minimum_required(VERSION 3.25) # FetchContent is available in 3.11+
project(BSTreeSimulator VERSION 0.0.1 LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

# Dependencies
set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR} EXCLUDE_FROM_ALL SYSTEM)
  endif()
endif()

# Our Project
set(bs_gcc_warns
    -Wshadow -Wundef -Wduplicated-cond -Wduplicated-branches -Wlogical-op
    -Wnull-dereference -Wswitch-default -Wuseless-cast -Wnon-virtual-dtor -Woverloaded-virtual
    -Wcast-align -Wcast-qual
)
set(bs_compile_options
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /Zc:preprocessor>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:GNU>:${bs_gcc_warns}>
    $<$<CXX_COMPILER_ID:Clang>:-fexperimental-library>  # C++20 `std::format()`
)

add_executable(${PROJECT_NAME})
add_subdirectory(src/)
# set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_compile_options(${PROJECT_NAME} PRIVATE ${bs_compile_options})

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

    install(FILES
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.js
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.wasm
        DESTINATION . )
    install(FILES
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.html
        RENAME index.html DESTINATION . )
else()
    add_subdirectory(tests/)
    add_test(NAME test_bstree COMMAND bstree_validate)
    add_test(NAME test_rbtree COMMAND rbtree_validate)
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
