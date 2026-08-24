cmake_minimum_required(VERSION 3.16)
project(graphsearch CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_executable(graphsearch
    src/main.cpp
    src/dijkstra.cpp
    src/astar.cpp
    src/generators.cpp
    src/manual_graphs.cpp
    src/stats.cpp
    src/tests.cpp
    src/benchmark.cpp
)

target_include_directories(graphsearch PRIVATE include)

# Older GCC (<9) requires linking libstdc++fs explicitly for <filesystem>;
# this is harmless to add unconditionally where the library exists, and a
# no-op / ignored where it does not.
include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_LIBRARIES stdc++fs)
check_cxx_source_compiles("#include <filesystem>\nint main(){ std::filesystem::path p(\".\"); return 0; }" HAVE_STDCXXFS)
unset(CMAKE_REQUIRED_LIBRARIES)
if(HAVE_STDCXXFS)
    target_link_libraries(graphsearch PRIVATE stdc++fs)
endif()
