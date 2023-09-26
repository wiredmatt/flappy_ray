cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(example)

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

set(ferox_VERSION 4.5.0)
find_package(ferox ${ferox_VERSION} QUIET) # QUIET or REQUIRED
if (NOT ferox_FOUND) # If there's none, fetch and build ferox
  include(FetchContent)
  FetchContent_Declare(
    ferox
    GIT_REPOSITORY
    https://github.com/c-krit/ferox
    GIT_TAG main
  )
  FetchContent_GetProperties(ferox)
  if (NOT ferox_POPULATED) # Have we downloaded ferox yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(ferox)
        
    add_library(ferox STATIC
      ${ferox_SOURCE_DIR}/src/broad-phase.c
      ${ferox_SOURCE_DIR}/src/collision.c
      ${ferox_SOURCE_DIR}/src/geometry.c
      ${ferox_SOURCE_DIR}/src/rigid-body.c
      ${ferox_SOURCE_DIR}/src/timer.c
      ${ferox_SOURCE_DIR}/src/world.c
    )
    target_include_directories(ferox PUBLIC
      ${ferox_SOURCE_DIR}/include
      ${ferox_SOURCE_DIR}/examples/include
    )
    target_include_directories(ferox PRIVATE
      ${ferox_SOURCE_DIR}/ferox/src/external
    )
  endif()
endif()

# Our Project

add_executable(${PROJECT_NAME} src/main.c)
#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib ferox)

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
