cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(flappy_ray)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 4.5.0)
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
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

set(FLECS_VERSION 3.2.7)
find_package(flecs ${FLECS_VERSION} QUIET) # QUIET or REQUIRED
if (NOT flecs_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    flecs
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/SanderMertens/flecs/archive/refs/tags/v${FLECS_VERSION}.tar.gz
  )
  FetchContent_GetProperties(flecs)
  if (NOT flecs_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(flecs)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${flecs_SOURCE_DIR} ${flecs_BINARY_DIR})
  endif()
endif()

# Our Project
add_executable(${PROJECT_NAME} src/main.c)
#set(raylib_VERBOSE 1)

# link dependencies so we can do #include "raylib.h" and "flecs.h"
target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(${PROJECT_NAME} flecs)


# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()