# flappy_ray

A Flappy Bird clone in Good Ol' C.

## Build for desktop

```
cmake -B build
cmake --build build

./build/flappy_ray # run
```

## Build for web with emscripten

[Get emscripten sdk](https://emscripten.org/)

```
mkdir webbuild
cd webbuild

emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXE_LINKER_FLAGS="-s USE_GLFW=3" -DCMAKE_EXECUTABLE_SUFFIX=".html"

emmake make

emrun example.html #run
```

## Make it yourself from scratch

```
mkdir flappy_ray
cd flappy_ray
mkdir src
touch CMakeLists.txt
touch src/main.c
```

CMake is actually very user friendly.

This code is essentially declaring a variable RAYLIB_VERSION with the version you want to use, it tries to find the package in your local cache and if it doesn't, it'll download it from github. That's it.


```CMake
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
```

This makes it so CMake knows what the entrypoint of the project is, and compiles it.
```CMake
add_executable(${PROJECT_NAME} src/main.c)
```

And with this, it makes the libraries accessible to your code, in src/\<file>.c
```CMake
target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(${PROJECT_NAME} flecs)
target_link_libraries(${PROJECT_NAME} chipmunk)
```

Only consideration for the dependencies of this project is Chipmunk. As you can read in its declaration, it contains an additional instruction the other two don't have: 

```CMake
include_directories(${chipmunk_SOURCE_DIR}/include)
```

Some C libraries are like this, the header is in a separate location and you need to manually specify it.

## Motivation

I've always hated Unity, Unreal takes 16 minutes to load for me, Godot crashes constantly and GDScript is just horrible. 
There are some good game frameworks out there, like FNA and Monogame, but they run with C#, which I also dislike.

This is a little experiment to see if going barebones is doable and worth it. So far, the absolute lack of concrete examples to integrate the different libraries (windowing, physics, ecs, sounds) makes it hard to figure things out, hopefully this'll become a good starting point for everyone interested in making games this way.

> "Seems like no one writes code anymore, they just glue together frameworks" - A random YouTube comment

> "Even Beethoven wrote his simphony in C" - Another random YouTube comment

> "Nothing Better than C" - Linus Torvalds